



function Test_Heat_CheckStop2Fail() {

    log("INFO: Test Check Stop - ERROR - FULL STOP", "info");
    stepSubscription = on({ id: HP_STEP }, function (obj) {
        var newStep = obj.state.val;
        var result;
        var res;
        unsubscribe(stepSubscription); // just one step checked
        setTimeout(function () {
            res = getState(HP_PUMP1).val == 0;
            if (!res) {
                log("Pump Floor 1 is running", "error");
                result = "Hardware error";
            } else {
                log("Pump floor 1 has stopped", "info");
                res = getState(HP_PUMP2).val == 0;
                if (!res) {
                    log("Pump Floor 2 is running", "error");
                    result = "Hardware error";
                } else {
                    log("Pump floor 2 has stopped", "info");
                    res = getState(HP_PUMP_TANKIN).val == 0;
                    if (!res) {
                        log("Pump TankIn is running", "error");
                        result = "Hardware error";
                    } else {
                        log("Pump TankIn has stopped", "info");
                        res = getState(HP_PUMP_TANKOUT).val == 0;
                        if (!res) {
                            log("Pump TankOut is running", "error");
                            result = "Hardware error";
                        } else {
                            log("Pump TankOut has stopped", "info");
                            res = getState(HP_PUMP_GND).val == 0;
                            if (!res) {
                                log("Pump Ground is running", "error");
                                result = "Hardware error";
                            } else {
                                log("Pump Ground has stopped", "info");
                                res = getState(HP_COMPRESSOR).val == 0;
                                if (!res) {
                                    log("Compressor running", "error");
                                    result = "Hardware error";
                                } else {
                                    log("Compressor has stopped", "info");
                                    if (newStep == 'Z') {
                                        result = "OK";
                                        log("PASSED! Test Check Stop - ERROR - FULL STOP", "info");
                                    } else {
                                        result = "Wrong Step";
                                        log("FAIL! New Step is: " + newStep, "error");
                                    }
                                }
                            }
                        }
                    }
                }
            }
            ResultProcessing(result);
        }, 3000);
    });


    timer = setTimeout(function () {
        clearTimeout(timer);
        ResultProcessing("Timeout");
    }, 10000);

    setState(HP_VOLTAGE_SWITCH, 0);
}




log("start!");
var nexStep;
var stepSubscription = null;
var alertSubscription = null;
/*
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
*/
var timer = null;

var steps = [];
steps.push(Test_Empty2HeatIdle,
    Test_Heat_Idle2Initial,
    Test_Heat_Initial2CheckStart,
    Test_Heat_CheckStart2StartGnd,
    Test_Heat_StartGnd2Heat,
    Test_Heat_Heat2CheckStop,
    Test_Heat_CheckStop2Fail,
    Finish);
var step = 0;
StartTest1();






