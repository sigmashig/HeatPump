/*


*/



log("start!");
var nexStep;
var stepSubscription = null;
var alertSubscription = $(HP_ALERT_CODE).on(function (obj) {
    var alertCode = obj.state.val;
    var result;
    var ind = obj.id.indexOf(".Code");
    var text = obj.id.substring(0, ind) + ".Text";
    if (alertCode == '') {
        result = "OK";
        log("PASSED! Alert disappeared", "info");
    } else {
        result = "ALERT";
        log("FAIL! Alert(" + alertCode + ") occured " + getState(text).val, "error");
    }
    ResultProcessing(result);
});

var timer = null;

var steps = [];
steps.push(Test_Empty2HeatIdle,
    Test_Heat_Idle2Initial,
    Test_Heat_Initial2CheckStart,
    Test_Heat_CheckStart2StartGnd,
    Test_Heat_StartGnd2Heat,
    Test_Heat_Heat2CheckStop,
    Test_Heat_CheckStop2StopCompressor,
    Test_Heat_StopCompressor2StopHeating,
    Test_Heat_StopHeating2CheckStart,
    Finish);
var step = 0;
StartTest1();






