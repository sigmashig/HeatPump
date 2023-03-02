/*


*/
const HP_ROOT = "mqtt.0.HeatPump";
const HP_BOARD = HP_ROOT + ".Board_07";
const HP_CONFIG = HP_BOARD + ".Config";
const HP_STATUS = HP_BOARD + ".Status";
const HP_ALERT = HP_BOARD + ".Alert";

const HP_COMMAND = HP_CONFIG + ".Command";
const HP_HEATCOLD = HP_CONFIG + ".HeatCold";
const HP_HYST = HP_CONFIG + ".Hysteresis";
const HP_HYSTGROUND = HP_CONFIG + ".HystGround";
const HP_HYSTHPROOM = HP_CONFIG + ".HystHpRoom";
const HP_MODE = HP_CONFIG + ".Mode";
const HP_MANUALTEMP = HP_CONFIG + ".ManualTemp";
const HP_SIMULATOR = HP_CONFIG + ".Simulator";
const HP_TIMEZONE = HP_CONFIG + ".TimeZone";

const HP_ISREADY = HP_CONFIG + ".IsReady";

const HP_STEP = HP_STATUS + ".Step";

const HP_ALERT_CODE = HP_ALERT + ".*" + ".Code";
const HP_ALERT_TEXT = HP_ALERT + ".*" + ".Text";

const HP_VOLTAGE_SWITCH = HP_STATUS + ".Contactor.VoltageSwitch";
const HP_PRESSURE_SWITCH = HP_STATUS + ".Contactor.PressureSwitch";


function SetInitialConfig() {
    setState(HP_COMMAND, 0); //NO cmd
    setState(HP_HEATCOLD, 0); //No Heat
    setState(HP_MODE, 0);       //No Mode
    setState(HP_HYST, 2);
    setState(HP_HYSTGROUND, 2);
    setState(HP_HYSTHPROOM, 5);
    setState(HP_MANUALTEMP, 19);
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

    log("INFO: Test Empty -> Idle is started", "info");
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
    alertSubscription = on({ id: HP_ALERT_CODE }, function (obj) {
        var alertCode = obj.state.val;
        var result;

        unsubscribe(alertSubscription);

        if (alertCode == '') {
            result = "OK";
            log("PASSED! Alert disappeared", "info");
        } else {
            result = "ALERT";
            log("FAIL! Alert(" + alertCode + ") occured " + getState(HP_ALERT_TEXT).val, "error");
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


function Test_HeatIdle2CheckStart() {

    log("INFO: Test Idle -> Check start is started", "info");
    stepSubscription = on({ id: HP_STEP }, function (obj) {
        var newStep = obj.state.val;
        var result;

        unsubscribe(stepSubscription); // just one step checked

        if (newStep == 'I') {
            result = "OK";
            log("PASSED! Transfer Idle -> Check Start", "info");
        } else {
            result = "Wrong Step";
            log("FAIL! New Step is: " + newStep, "error");
        }
        ResultProcessing(result);
    });
    alertSubscription = on({ id: HP_ALERT_CODE }, function (obj) {
        var alertCode = obj.state.val;
        var result;

        unsubscribe(alertSubscription);

        if (alertCode == '') {
            result = "OK";
            log("PASSED! Alert disappeared", "info");
        } else {
            result = "ALERT";
            log("FAIL! Alert(" + alertCode + ") occured " + getState(HP_ALERT_TEXT).val, "error");
        }
        ResultProcessing(result);
    });

    timer = setTimeout(function () {
        clearTimeout(timer);

        ResultProcessing("Timeout");
    }, 5000);

    setState(HP_COMMAND, 1); // Run
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
    SetInitialConfig();
}

log("start!");

var nexStep;
var stepSubscription = null;
var alertSubscription = null;
var timer = null;

var steps = [];
steps.push(Test_Empty2HeatIdle, Test_HeatIdle2CheckStart, Finish);
var step = 0;
StartTest1();







