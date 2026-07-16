/** HISE Moonbase Bridge

Copyright 2026 Christoph Hart

This file is not part of the open source HISE code base, but only accessible for Moonbase clients.

*/

#include "h2m_helpers.h"

namespace hise {
using namespace juce;

moonbase::licensing_options MoonbaseHelpers::fromVar(const var& options, const String& rsaKey)
{
	moonbase::licensing_options o;

	o.endpoint = options["endpoint"].toString().toStdString();
	o.product_id = options["product_id"].toString().toStdString();

#if HISE_USE_MOONBASE_WITH_HISE_MANAGED
	o.client_info = "HISE Managed License";
#else
	o.client_info = "HISE Moonbase Bridge";
#endif

	o.public_key = rsaKey.toStdString();

	return o;
}

juce::var MoonbaseHelpers::toVar(moonbase::juce_bridge::MoonbaseUnlockStatus::RevokeOutcome r)
{
	StringArray sa = {
			"Revoked",
			"NoLicense",
			"NotRevokable",
			"Unreachable"
	};

	return sa[(int)r];
}

juce::var MoonbaseHelpers::toVar(const moonbase::license& value)
{
	juce::DynamicObject::Ptr result = new juce::DynamicObject();

	result->setProperty("id", toJuceString(value.id));
	result->setProperty("activation_id", toJuceString(value.activation_id));
	result->setProperty("trial", value.trial);
	result->setProperty("method", toString(value.method));
	result->setProperty("licensed_product", productToVar(value.licensed_product));
	result->setProperty("issued_to", userToVar(value.issued_to));
	result->setProperty("issued_at", toISO8601(value.issued_at));
	result->setProperty(
		"expires_at",
		value.expires_at
		? juce::var(toISO8601(*value.expires_at))
		: juce::var());
	result->setProperty("validated_at", toISO8601(value.validated_at));

	juce::Array<juce::var> ownedSubProductIds;
	ownedSubProductIds.ensureStorageAllocated(
		static_cast<int>(value.owned_sub_product_ids.size()));

	for (const auto& id : value.owned_sub_product_ids)
		ownedSubProductIds.add(toJuceString(id));

	result->setProperty("owned_sub_product_ids", ownedSubProductIds);
	result->setProperty(
		"subscription_id",
		value.subscription_id
		? juce::var(toJuceString(*value.subscription_id))
		: juce::var());
	result->setProperty(
		"seat_count",
		value.seat_count
		? juce::var(static_cast<juce::int64>(*value.seat_count))
		: juce::var());
	result->setProperty(
		"seats_used",
		value.seats_used
		? juce::var(static_cast<juce::int64>(*value.seats_used))
		: juce::var());
	result->setProperty("properties", jsonToVar(value.properties));
	result->setProperty("token", toJuceString(value.token));

	return var(result.get());
}

juce::var MoonbaseHelpers::toVar(const moonbase::juce_bridge::MoonbaseUnlockStatus::AsyncValidationResult& r)
{
	DynamicObject::Ptr result = new DynamicObject();

	result->setProperty("outcome", (int)r.outcome);

	if (r.license.has_value())
		result->setProperty("license", toVar(r.license.value()));

	return var(result.get());
}

juce::String MoonbaseHelpers::toJuceString(const std::string& value)
{
	return juce::String::fromUTF8(value.data(), static_cast<int>(value.size()));
}

juce::String MoonbaseHelpers::toISO8601(const std::chrono::system_clock::time_point& value)
{
	using namespace std::chrono;

	const auto wholeSeconds = floor<seconds>(value);
	const auto millisecondsPart = duration_cast<milliseconds>(value - wholeSeconds).count();
	const auto time = system_clock::to_time_t(wholeSeconds);

	std::tm utcTime{};

#if JUCE_WINDOWS
	gmtime_s(&utcTime, &time);
#else
	gmtime_r(&time, &utcTime);
#endif

	return juce::String::formatted(
		"%04d-%02d-%02dT%02d:%02d:%02d.%03dZ",
		utcTime.tm_year + 1900,
		utcTime.tm_mon + 1,
		utcTime.tm_mday,
		utcTime.tm_hour,
		utcTime.tm_min,
		utcTime.tm_sec,
		static_cast<int>(millisecondsPart));
}

juce::String MoonbaseHelpers::toString(moonbase::activation_method method)
{
	switch (method)
	{
	case moonbase::activation_method::online:
		return "online";

	case moonbase::activation_method::offline:
		return "offline";
	}

	jassertfalse;
	return {};
}

juce::var MoonbaseHelpers::jsonToVar(const nlohmann::json& value)
{
	if (value.is_null())
		return {};

	if (value.is_boolean())
		return value.get<bool>();

	if (value.is_number_integer())
		return static_cast<juce::int64>(value.get<std::int64_t>());

	if (value.is_number_unsigned())
	{
		const auto unsignedValue = value.get<std::uint64_t>();

		if (unsignedValue <= static_cast<std::uint64_t>(
			std::numeric_limits<juce::int64>::max()))
		{
			return static_cast<juce::int64>(unsignedValue);
		}

		// juce::var has no unsigned 64-bit type.
		return static_cast<double>(unsignedValue);
	}

	if (value.is_number_float())
		return value.get<double>();

	if (value.is_string())
		return toJuceString(value.get_ref<const std::string&>());

	if (value.is_array())
	{
		juce::Array<juce::var> result;
		result.ensureStorageAllocated(static_cast<int>(value.size()));

		for (const auto& item : value)
			result.add(jsonToVar(item));

		return result;
	}

	if (value.is_object())
	{
		auto* result = new juce::DynamicObject();

		for (auto it = value.begin(); it != value.end(); ++it)
			result->setProperty(toJuceString(it.key()), jsonToVar(it.value()));

		return juce::var(result);
	}

	jassertfalse;
	return {};
}

juce::var MoonbaseHelpers::productToVar(const moonbase::product& value)
{
	auto* result = new juce::DynamicObject();

	result->setProperty("id", toJuceString(value.id));
	result->setProperty("name", toJuceString(value.name));
	result->setProperty(
		"current_release_version",
		value.current_release_version
		? juce::var(toJuceString(*value.current_release_version))
		: juce::var());
	result->setProperty("properties", jsonToVar(value.properties));

	return juce::var(result);
}

juce::var MoonbaseHelpers::userToVar(const moonbase::user& value)
{
	auto* result = new juce::DynamicObject();

	result->setProperty("id", toJuceString(value.id));
	result->setProperty("name", toJuceString(value.name));
	result->setProperty("email", toJuceString(value.email));
	result->setProperty("properties", jsonToVar(value.properties));

	return juce::var(result);
}

size_t RsaPemHelpers::readDerLength(const uint8_t* data, size_t size, size_t& pos)
{
	if (pos >= size)
		throw std::runtime_error("Unexpected end of DER");

	auto b = data[pos++];

	if ((b & 0x80) == 0)
		return b;

	const int numBytes = b & 0x7f;

	if (numBytes <= 0 || numBytes > 4)
		throw std::runtime_error("Unsupported DER length");

	size_t len = 0;

	for (int i = 0; i < numBytes; ++i)
	{
		if (pos >= size)
			throw std::runtime_error("Unexpected end of DER");

		len = (len << 8) | data[pos++];
	}

	return len;
}

juce::String RsaPemHelpers::readDerIntegerHex(const uint8_t* data, size_t size, size_t& pos)
{
	if (pos >= size || data[pos++] != 0x02)
		throw std::runtime_error("Expected DER INTEGER");

	auto len = readDerLength(data, size, pos);

	if (pos + len > size)
		throw std::runtime_error("Invalid DER INTEGER length");

	// Strip leading zero used to force positive INTEGER.
	if (len > 0 && data[pos] == 0x00)
	{
		++pos;
		--len;
	}

	juce::String hex;

	for (size_t i = 0; i < len; ++i)
		hex << juce::String::toHexString((int)data[pos++]).paddedLeft('0', 2);

	return hex;
}

void RsaPemHelpers::writeDerLength(juce::MemoryOutputStream& out, size_t len)
{
	if (len < 0x80)
	{
		out.writeByte((char)len);
		return;
	}

	uint8_t bytes[8];
	int numBytes = 0;

	while (len > 0)
	{
		bytes[numBytes++] = static_cast<uint8_t> (len & 0xff);
		len >>= 8;
	}

	out.writeByte((char)(0x80 | numBytes));

	for (int i = numBytes - 1; i >= 0; --i)
		out.writeByte((char)bytes[i]);
}

juce::MemoryBlock RsaPemHelpers::hexToBytes(juce::String hex)
{
	hex = hex.removeCharacters(" \t\r\n");

	// Important: JUCE BigInteger::toString(16) can return odd-length hex,
	// eg exponent 65537 as "10001" instead of "010001".
	if ((hex.length() & 1) != 0)
		hex = "0" + hex;

	juce::MemoryBlock result;

	for (int i = 0; i < hex.length(); i += 2)
	{
		auto byteString = hex.substring(i, i + 2);
		auto byte = static_cast<uint8_t>(byteString.getHexValue32());
		result.append(&byte, 1);
	}

	return result;
}

void RsaPemHelpers::writeDerInteger(juce::MemoryOutputStream& out, juce::String hex)
{
	auto bytes = hexToBytes(hex);

	const auto* data = static_cast<const uint8_t*> (bytes.getData());
	auto size = bytes.getSize();

	const bool needsLeadingZero = size > 0 && (data[0] & 0x80) != 0;

	out.writeByte((char)0x02);
	writeDerLength(out, size + (needsLeadingZero ? 1 : 0));

	if (needsLeadingZero)
		out.writeByte(0x00);

	out.write(data, size);
}

juce::String RsaPemHelpers::juceKeyToRsaPublicKeyPem(const juce::RSAKey& key)
{
	// JUCE serialises RSAKey as two comma-separated hex parts.
	const auto parts = juce::StringArray::fromTokens(key.toString(), ",", "");

	if (parts.size() != 2)
		throw std::runtime_error("Unexpected JUCE RSAKey format");

	const auto exponentHex = parts[0];
	const auto modulusHex = parts[1];

	juce::MemoryOutputStream body;
	writeDerInteger(body, modulusHex);
	writeDerInteger(body, exponentHex);

	juce::MemoryOutputStream der;
	der.writeByte((char)0x30);
	writeDerLength(der, body.getDataSize());
	der.write(body.getData(), body.getDataSize());

	auto base64 = juce::Base64::toBase64(der.getData(), der.getDataSize());

	juce::String wrapped;
	for (int i = 0; i < base64.length(); i += 64)
		wrapped << base64.substring(i, i + 64) << "\n";

	return "-----BEGIN RSA PUBLIC KEY-----\n"
		+ wrapped
		+ "-----END RSA PUBLIC KEY-----";
}

}