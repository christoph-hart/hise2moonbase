/** HISE Moonbase Bridge

Copyright 2026 Christoph Hart

This file is not part of the open source HISE code base, but only accessible for Moonbase clients.

*/

#pragma once

#include <JuceHeader.h>

namespace hise {
using namespace juce;

// Use these as int opcode for controlling which operation to perform
// you can either use magic numbers or named constants in your scripting code depending on your 
// security philosophy
enum class OpType
{
	Initialisation,
	Register,
	Activate,
	Revoke,
	SaveLicense,
	LoadLicense,
	numOpTypes
};

void MoonbaseUnlocker::RefObject::timerCallback()
{
	using MU = moonbase::juce_bridge::MoonbaseUnlockStatus;
	auto fp = dynamic_cast<FrontendProcessor*>(getScriptProcessor()->getMainController_());
	auto unlocker = dynamic_cast<MU*>(fp->unlocker.getUnlockerObject());

	if (unlocker->pollPendingActivation())
	{
		stopTimer();
		
		if (auto l = unlocker->moonbaseLicense())
		{
			licensingCallback.call1(MoonbaseHelpers::toVar(l.value()));
		}
	}
}

void MoonbaseUnlocker::RefObject::performMoonbaseOp(int opType_, const var& licensingOptions, const var& callback)
{
	auto opType = (OpType)opType_;

	auto fp = dynamic_cast<FrontendProcessor*>(getScriptProcessor()->getMainController_());

	using MU = moonbase::juce_bridge::MoonbaseUnlockStatus;

	auto unlocker = dynamic_cast<MU*>(fp->unlocker.getUnlockerObject());

	if (opType == OpType::Initialisation)
	{
		auto rsa = fp->unlocker.getPublicKey();

		const auto parts = juce::StringArray::fromTokens(rsa.toString(), ",", "");
		const auto exponentHex = parts[0];
		const auto modulusHex = parts[1];

		auto key = RsaPemHelpers::juceKeyToRsaPublicKeyPem(rsa);

		auto useKey = (bool)licensingOptions.getProperty("use_keyfile", true);

		auto p = FrontendHandler::getAppDataDirectory(nullptr).getChildFile("license.mb").getFullPathName().toStdString();
		std::shared_ptr<moonbase::file_license_store> store = useKey ? std::make_shared<moonbase::file_license_store>(p) : nullptr;

		auto n = new MU(MoonbaseHelpers::fromVar(licensingOptions, key), store);
		fp->unlocker.unlocker = n;
	}
	if (opType == OpType::Activate)
	{
		if (unlocker != nullptr)
		{
			licensingCallback = WeakCallbackHolder(getScriptProcessor(), this, callback, 1);
			licensingCallback.incRefCount();

			unlocker->tryLoadStoredLicenseAsync([callback, this](MU::AsyncValidationResult result)
			{
				this->licensingCallback.call1(MoonbaseHelpers::toVar(result));
			});
		}
	}
	if (opType == OpType::Register)
	{
		if (unlocker != nullptr && !unlocker->isUnlocked())
		{
			licensingCallback = WeakCallbackHolder(getScriptProcessor(), this, callback, 1);
			licensingCallback.incRefCount();

			try
			{
				auto url = unlocker->beginActivation();
				url.launchInDefaultBrowser();

				startTimer(1000);
				licensingCallback.call1("waiting for activation in browser...");
			}
			catch (const std::exception& ex)
			{
				licensingCallback.call1(juce::String("Activation request failed: ") + ex.what());
			}
		}
	}
	if (opType == OpType::Revoke)
	{
		if (unlocker != nullptr)
		{
			licensingCallback = WeakCallbackHolder(getScriptProcessor(), this, callback, 1);
			licensingCallback.incRefCount();

			unlocker->revokeActivationAsync([callback, this](MU::RevokeOutcome result)
			{
				this->licensingCallback.call1(MoonbaseHelpers::toVar(result));
			});
		}
	}
	if (opType == OpType::SaveLicense)
	{
		if (unlocker != nullptr)
		{

		}
	}
	if (opType == OpType::LoadLicense)
	{
		if (unlocker != nullptr)
		{

		}
	}

}

// this needs to be defined in order to prevent a linker error on macOS
juce::RSAKey ScriptUnlocker::getPublicKey()
{
    return RSAKey();
}

} // namespace hise
