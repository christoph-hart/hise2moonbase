# HISE2Moonbase Integration

This repository contains the C++ code and a test HISE project to integrate the Moonbase SDK into a HISE plugin.

## How to build this project

1. Clone this project and its submodules (the moonbase SDK is a separate submodule)
1. Start up HISE 4.9.3+
2. Load this project /XmlPresetBackups/MoonbaseTest
3. Compile (standalone or plugin)
4. Test the actions provided on the plugin interface.

## How to include this in your project:

### Transfer Files

Copy all the files in AdditionalSourceCode (including the submodules) into your HISE project's AdditionalSource folder.

### Setup the preprocessors

Set these preprocessor flags for each platform you support (macOS / Windows)

```cpp
USE_COPY_PROTECTION=1
USE_SCRIPT_COPY_PROTECTION=1
HISE_USE_MOONBASE=1
HISE_DEACTIVATE_OVERLAY=1
HISE_USE_MOONBASE_WITH_HISE_MANAGED=1|0
```

Note that the last preprocessor must be set to `1` or `0` depending on whether you are using the HISE managed licensing tier or not.

> The rationale for this is that the HISE Managed license usually requires the usage of HISE Activate for license tracking, but with this preprocessor set to 1 you can also use Moonbase and their additional functions if desired. This preprocessor ensures that the licenses are tracked the same way as with the HISE Activate server and must be set correctly in order to comply with the license agreement of HISE Managed.

### Integrate the Moonbase Activations

The Moonbase functions are accessible via the `ScriptUnlocker` object which reuses most of the copy protection functions that HISE Activate offers but allows the Moonbase server to unlock the plugin / handle the licensing.

```javascript
const var unlocker = Engine.createLicenseUnlocker();

/** A callback that will be executed asynchronously after each operation completes. */
inline function onMoonbaseResponse(obj)
{
	// obj is a JSON with the license details or a simple string
	// depending on the operation
	Console.print(trace(obj));
};

// All operations are funneled through this single scripting API call which takes
// a integer number that defines the operation, a JSON blob for input data and a callback.
ul.performMoonbaseOp(OP_TYPE, { someData: 1234 }, onMoonbaseResponse);
```

Currently these Moonbase operations are supported:

| Operation | OpType | Description | Input argument | Callback parameter |
| --- | --- | --- | --- | --- |
| **ConvertRSA** | `99` | Call this once in the HISE backend to create the `RSA.xml` keyfile that HISE will embed & obfuscate into your plugin. Note that this is a noop in the compiled plugin but it's highly recommended to delete that line once the RSA.xml file is created, otherwise the RSA key would be embedded as plain text in your script. | Your product-specific RSA key as PEM Key string. | nothing |
| **Initialisation** | `0` | Call this in the onInit callback to initialise the licensing system. | The product data as JSON (see below). | nothing. |
| **Activate** | `2` | Try to activate the plugin with a previously registered license. You can try this at startup and show a UI element to register if it fails. | Nothing | A JSON element with the full licensing details. (see below). |
| **Register** | `1` | If the activation call fails, this will open the webbrowser and starts the Moonbase authentification. Once this is completed, the callback will be executed with the result. | Nothing | A JSON element with the full licensing details. (see below). |
| **Revoke** | `3` | Deactivates the license on this computer. | Nothing | A simple string with the revoke result. |

You can integrate these calls into your plugin interface as you see fit, but the recommended way to use these are:

- inititialisation and activation go into the onInit script
- register and revoke go into dedicated button callbacks

Here are two example JSON data blobs for the input / response from Moonbase. 

**Initialisation Data:**

```json
{
	"endpoint":   "https://demo.moonbase.sh", // the moonbase URL
	"product_id": "demo-app",				  // your product id
	"account_id": "account-id",			  // your account id
	"use_keyfile": true                     // optional, defaults to true
}
```

**Licensing data** (Response from Moonbase)

```json
{
  "id": "7265afec13bccb06eed007aafb7c996c",
  "activation_id": "7265afec13bccb06eed007aafb7c996c",
  "trial": true,
  "method": "online",
  "licensed_product": {
    "id": "demo-app",
    "name": "Demo App",
    "current_release_version": "1.0.0",
    "properties": {
    }
  },
  "issued_to": {
    "id": "00000000-0000-0000-0000-000000000000",
    "name": "anonymous",
    "email": "anonymous",
    "properties": {
    }
  },
  "issued_at": "2026-07-15T14:22:54.000Z",
  "expires_at": "2026-07-29T14:22:54.000Z",
  "validated_at": "2026-07-16T12:58:16.000Z",
  "owned_sub_product_ids": [],
  "subscription_id": null,
  "seat_count": null,
  "seats_used": null,
  "properties": {
  },
  "token": "XXX"
}
```

> Note that you don't need to do anything to lock down the plugin as this is handled by the unlocker automatically, but you can use that data to display it to the user.

