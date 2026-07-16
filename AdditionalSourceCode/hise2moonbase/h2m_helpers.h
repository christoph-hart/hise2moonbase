/** HISE Moonbase Bridge

Copyright 2026 Christoph Hart

This file is not part of the open source HISE code base, but only accessible for Moonbase clients.

*/

#pragma once

#include <JuceHeader.h>

namespace hise {
using namespace juce;

/** Helpers classes to serialize the moonbase licensing states to a juce::var. */
struct MoonbaseHelpers
{
	static moonbase::licensing_options fromVar(const var& options, const String& rsaKey);

	static var toVar(moonbase::juce_bridge::MoonbaseUnlockStatus::RevokeOutcome r);
	static var toVar(const moonbase::license& value);
	static var toVar(const moonbase::juce_bridge::MoonbaseUnlockStatus::AsyncValidationResult& r);

private:

	static juce::String toJuceString(const std::string& value);
	static juce::String toISO8601(const std::chrono::system_clock::time_point& value);
	static juce::String toString(moonbase::activation_method method);
	static juce::var jsonToVar(const nlohmann::json& value);
	static juce::var productToVar(const moonbase::product& value);
	static juce::var userToVar(const moonbase::user& value);

};

/** Helper classes to convert the REM public RSA key into the JUCE RSA key stored at RSA.xml. */
namespace RsaPemHelpers
{
	static size_t readDerLength(const uint8_t* data, size_t size, size_t& pos);
	static juce::String readDerIntegerHex(const uint8_t* data, size_t size, size_t& pos);
	static void writeDerLength(juce::MemoryOutputStream& out, size_t len);
	static juce::MemoryBlock hexToBytes(juce::String hex);
	static void writeDerInteger(juce::MemoryOutputStream& out, juce::String hex);
	static juce::String juceKeyToRsaPublicKeyPem(const juce::RSAKey& key);
};

} // namespace hise