Content.makeFrontInterface(600, 600);

const var ul = Engine.createLicenseUnlocker();

/*
ul.performMoonbaseOp(99, "-----BEGIN RSA PUBLIC KEY-----
MIIBCgKCAQEAutOqeUiPMgYjAwQ53CyKhJSqojr2bejce0CshQi9Hd8mNZbkoROx
oS56eIzehFSlX4YwHnF47AR1+fPOe7Q33Cgzd6d9xqksiMH7sWK2mADIlB66vZdW
uk3Me0UMB22Biy1RQbSRMivu79MxCofsympoL/5CFjJLd1u37kxjuRWVLjJS84Rr
3L2W7R7Exnno/giC+L/Dv711mjgstmtlAQm5ZINvFvoLA1eFTDs6nlCs3dpJSiq3
fsBUMT9FtudzS5As54jeT/8MB66fJJ0A1LQ/v5CW8ACQYseFSIoOKErD3xU7QLIJ
ERUn++6CVMPvZo67jVbTY+GCXYfW4gGVZQIDAQAB
-----END RSA PUBLIC KEY-----", "");
*/

const var Label1 = Content.getComponent("Label1");

namespace MoonbaseActions
{
	const var INITIALISE = 0;
	const var REGISTER = 1;
	const var ACTIVATE = 2;
	const var REVOKE = 3;
	const var SAVE_RSA = 99;
	
	const var ACTIONS = [ REGISTER, ACTIVATE, REVOKE, SAVE_RSA ];
}

const var Buttons = [Content.getComponent("RegisterButton"),
					 Content.getComponent("ActivateButton"),
                     Content.getComponent("RevokeButton")];

const var MOONBASE_DATA = {
	endpoint: "https://demo.moonbase.sh",
	product_id: "demo-app",
	account_id: "account-id"
};

ul.performMoonbaseOp(MoonbaseActions.INITIALISE, MOONBASE_DATA, onMoonbaseResponse);

inline function onButton(component, value)
{
	local action = MoonbaseActions.ACTIONS[Buttons.indexOf(component)];
	ul.performMoonbaseOp(action, MOONBASE_DATA, onMoonbaseResponse);
}

for(b in Buttons)
	b.setControlCallback(onButton);
                     
inline function onMoonbaseResponse(obj)
{
	Label1.set("bgColour", ul.isUnlocked() ? Colours.green : Colours.red);
	Label1.set("text", trace(obj));
};

onMoonbaseResponse("not initialised");


function onNoteOn()
{
	
}
 function onNoteOff()
{
	
}
 function onController()
{
	
}
 function onTimer()
{
	
}
 function onControl(number, value)
{
	
}
 