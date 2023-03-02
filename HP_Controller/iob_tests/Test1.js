/*


*/
const HP_ROOT = "mqtt.0.HeatPump";
const HP_BOARD = HP_ROOT + ".Board_07";
const HP_CONFIG = HP_BOARD + ".Config";
const HP_STATUS = HP_BOARD + ".Status";
const HP_ALERT = HP_BOARD + ".Alert";

const HP_TEMP = HP_STATUS + ".Temperature";
const HP_RELAY = HP_STATUS + ".Relay";

const HP_COMMAND = HP_CONFIG + ".Command";
const HP_HEATCOLD = HP_CONFIG + ".HeatCold";
const HP_HYST = HP_CONFIG + ".Hysteresis";
const HP_HYSTGROUND = HP_CONFIG + ".HystGround";
const HP_HYSTHPROOM = HP_CONFIG + ".HystHpRoom";
const HP_MODE = HP_CONFIG + ".Mode";
const HP_MANUALTEMP = HP_CONFIG + ".ManualTemp";
const HP_DESIREDTEMP = HP_CONFIG + ".DesiredTemp";
const HP_SIMULATOR = HP_CONFIG + ".Simulator";
const HP_TIMEZONE = HP_CONFIG + ".TimeZone";

const HP_ISREADY = HP_CONFIG + ".IsReady";

const HP_STEP = HP_STATUS + ".Step";

const HP_ALERT_CODE = HP_ALERT + ".*" + ".Code";
const HP_ALERT_TEXT = HP_ALERT + ".*" + ".Text";

const HP_VOLTAGE_SWITCH = HP_STATUS + ".Contactor.VoltageSwitch";
const HP_PRESSURE_SWITCH = HP_STATUS + ".Contactor.PressureSwitch";
const HP_PUMP1 = HP_RELAY + ".PumpFloor1";
const HP_PUMP2 = HP_RELAY + ".PumpFloor2";
const HP_PUMP_TANKIN = HP_RELAY + ".PumpTankIn";
const HP_PUMP_TANKOUT = HP_RELAY + ".PumpTankOut";
const HP_PUMP_GND = HP_RELAY + ".PumpGnd";
const HP_COMPRESSOR = HP_RELAY + ".Compressor";

const HP_TEMP_INSIDE = HP_TEMP + ".Tinside";

function SetInitialConfig() {
    setState(HP_COMMAND, 0); //NO cmd
    setState(HP_HEATCOLD, 0); //No Heat
    setState(HP_MODE, 0);       //No Mode
    setState(HP_HYST, 2);
    setState(HP_HYSTGROUND, 2);
    setState(HP_HYSTHPROOM, 5);
    setState(HP_MANUALTEMP, 19);
    setState(HP_DESIREDTEMP, 19);
    setState(HP_SIMULATOR, 1);
    setState(HP_TIMEZONE, "EET");

    setState(HP_VOLTAGE_SWITCH, 1);
    setState(HP_PRESSURE_SWITCH, 1);

}

function StartTest1() {
    var isReady = getState(HP_ISREADY).val;

    log("isReady=" + isReady);
    if (isReady == '1') {
        SetInitialConfig();
        step = 0;
        ResultProcessing("");
    }
}

function Test_Empty2HeatIdle() {

    log("INFO: Test Empty -> Idle started", "info");
    stepSubscription = on({ id: HP_STEP }, function (obj) {
        var newStep = obj.state.val;
        var result;

        unsubscribe(stepSubscription); // just one step checked

        if (newStep == '0') {
            result = "OK";
            log("PASSED! Transfer Empty -> Idle", "info");
        } else {
            result = "Wrong Step";
            log("FAIL! New Step is: " + newStep, "error");
        }
        ResultProcessing(result);
    });

    timer = setTimeout(function () {
        clearTimeout(timer);

        ResultProcessing("Timeout");
    }, 5000);

    setState(HP_HEATCOLD, 1); // Heat
    setState(HP_COMMAND, 0); // No Cmd
}


function Test_Heat_Idle2Initial() {

    log("INFO: Test Idle -> Initial started", "info");
    stepSubscription = on({ id: HP_STEP }, function (obj) {
        var newStep = obj.state.val;
        var result;

        unsubscribe(stepSubscription); // just one step checked

        if (newStep == 'I') {
            result = "OK";
            log("PASSED! Transfer Idle -> Initial", "info");
        } else {
            result = "Wrong Step";
            log("FAIL! New Step is: " + newStep, "error");
        }
        ResultProcessing(result);
    });

    timer = setTimeout(function () {
        clearTimeout(timer);

        ResultProcessing("Timeout");
    }, 5000);

    setState(HP_COMMAND, 1); // Run
}


function Test_Heat_Initial2CheckStart() {

    log("INFO: Test Initial -> Check started", "info");
    stepSubscription = on({ id: HP_STEP }, function (obj) {
        var newStep = obj.state.val;
        var result;
        unsubscribe(stepSubscription); // just one step checked
        if (getState(HP_PUMP1).val == 1 && getState(HP_PUMP2).val == 1 && getState(HP_PUMP_TANKOUT).val == 1) {
            log("All pumps started", "info");
            if (newStep == 'S') {
                result = "OK";
                log("PASSED! Transfer Initial -> Check", "info");
            } else {
                result = "Wrong Step";
                log("FAIL! New Step is: " + newStep, "error");
            }
        } else {
            result = "Hardware Error";
            log("Some pumps don't started");
        }
        ResultProcessing(result);
    });

    timer = setTimeout(function () {
        clearTimeout(timer);

        ResultProcessing("Timeout");
    }, 5000);

    //setState(HP_COMMAND,1); // Run
}

function Test_Heat_CheckStart2StartGnd() {

    log("INFO: Test Check -> Gnd started", "info");
    stepSubscription = on({ id: HP_STEP }, function (obj) {
        var newStep = obj.state.val;
        var result;
        unsubscribe(stepSubscription); // just one step checked
        if (getState(HP_PUMP_GND).val == 1 && getState(HP_PUMP_TANKIN).val == 1) {
            log("All pumps started", "info");
            if (newStep == 'G') {
                result = "OK";
                log("PASSED! Transfer Check -> Gnd", "info");
            } else {
                result = "Wrong Step";
                log("FAIL! New Step is: " + newStep, "error");
            }
        } else {
            result = "Hardware Error";
            log("Some pumps do not started");
        }
        ResultProcessing(result);
    });


    timer = setTimeout(function () {
        clearTimeout(timer);

        ResultProcessing("Timeout");
    }, 5000);
    setState(HP_HYST, 2);
    setState(HP_DESIREDTEMP, 19);
    setState(HP_TEMP_INSIDE, 16); // 19 - 2 = 17
}

function Test_Heat_StartGnd2Heat() {

    log("INFO: Test Gnd -> Heat started", "info");
    stepSubscription = on({ id: HP_STEP }, function (obj) {
        var newStep = obj.state.val;
        var result;
        unsubscribe(stepSubscription); // just one step checked
        if (getState(HP_COMPRESSOR).val == 1) {
            log("Compressor Started", "info");
            if (newStep == 'H') {
                result = "OK";
                log("PASSED! Transfer Gnd -> Heat", "info");
            } else {
                result = "Wrong Step";
                log("FAIL! New Step is: " + newStep, "error");
            }
        } else {
            result = "Hardware Error";
            log("Compressor do not started");
        }
        ResultProcessing(result);
    });


    timer = setTimeout(function () {
        clearTimeout(timer);
        ResultProcessing("Timeout");
    }, 5000);

    //    setState(HP_TEMP_INSIDE,16); // 19 - 2 = 17
}

function Test_Heat_Heat2StopCompressor() {

    log("INFO: Test Heat -> Stop  started", "info");
    stepSubscription = on({ id: HP_STEP }, function (obj) {
        var newStep = obj.state.val;
        var result;
        unsubscribe(stepSubscription); // just one step checked
        if (newStep == 'C') {
            result = "OK";
            log("PASSED! Transfer Heat -> Stop", "info");
        } else {
            result = "Wrong Step";
            log("FAIL! New Step is: " + newStep, "error");
        }
        ResultProcessing(result);
    });


    timer = setTimeout(function () {
        clearTimeout(timer);
        ResultProcessing("Timeout");
    }, 5000);

    setState(HP_HYST, 2);
    setState(HP_DESIREDTEMP, 19);
    setState(HP_TEMP_INSIDE, 22); // 19 + 2 = 21
}

function Test_Heat_StopCompressor2StopHeating() {

    log("INFO: Test Stop Compressor -> Stop Heating started", "info");
    stepSubscription = on({ id: HP_STEP }, function (obj) {
        var newStep = obj.state.val;
        var result;
        unsubscribe(stepSubscription); // just one step checked
        if (getState(HP_COMPRESSOR).val == 0) {
            log("Compressor Stoped", "info");
            if (newStep == 'T') {
                result = "OK";
                log("PASSED! Transfer Stop Compressor -> Stop Heating", "info");
            } else {
                result = "Wrong Step";
                log("FAIL! New Step is: " + newStep, "error");
            }
        } else {
            result = "Hardware Error";
            log("Compressor does not stoped");
        }
        ResultProcessing(result);
    });


    timer = setTimeout(function () {
        clearTimeout(timer);
        ResultProcessing("Timeout");
    }, 5000);

    //    setState(HP_TEMP_INSIDE,16); // 19 - 2 = 17
}

function Test_Heat_StopHeating2CheckStart() {

    log("INFO: Test Stop Heating -> Check Start started", "info");
    stepSubscription = on({ id: HP_STEP }, function (obj) {
        var newStep = obj.state.val;
        var result;
        unsubscribe(stepSubscription); // just one step checked
        if (getState(HP_PUMP_GND).val == 0 && getState(HP_PUMP_TANKIN).val == 0) {
            log("Pumps Stoped", "info");
            if (newStep == 'S') {
                result = "OK";
                log("PASSED! Transfer Stop Heating -> Check Start", "info");
            } else {
                result = "Wrong Step";
                log("FAIL! New Step is: " + newStep, "error");
            }
        } else {
            result = "Hardware Error";
            log("Pumps do not stoped");
        }
        ResultProcessing(result);
    });


    timer = setTimeout(function () {
        clearTimeout(timer);
        ResultProcessing("Timeout");
    }, 5000);

    //    setState(HP_TEMP_INSIDE,16); // 19 - 2 = 17
}


function ResultProcessing(result) {

    if (result == "") {
        steps[step]();
    } else {
        if (stepSubscription != null) {
            unsubscribe(stepSubscription);
            stepSubscription = null;
        }

        if (alertSubscription != null) {
            unsubscribe(alertSubscription);
            alertSubscription = null;
        }
        if (timer != null) {
            clearTimeout(timer);
            timer = null;
        }

        if (result != "OK") {
            log("FAILED! Result:" + result, "error");
            Finish();
        } else {
            log("PASSED! Step " + step, "info");
            step++;
            if (step < steps.length) {
                steps[step]();
            }
        }
    }
}


function Finish() {
    log("OK! Test suite finished");
    //unsubscribe(alertSubscription);
    SetInitialConfig();
}

log("start!");

var nexStep;
var stepSubscription = null;
var alertSubscription = on({ id: HP_ALERT_CODE }, function (obj) {
    var alertCode = obj.state.val;
    var result;

    if (alertCode == '') {
        result = "OK";
        log("PASSED! Alert disappeared", "info");
    } else {
        result = "ALERT";
        log("FAIL! Alert(" + alertCode + ") occured " + getState(HP_ALERT_TEXT).val, "error");
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
    Test_Heat_Heat2StopCompressor,
    Test_Heat_StopCompressor2StopHeating,
    Test_Heat_StopHeating2CheckStart,
    Finish);
var step = 0;
StartTest1();







