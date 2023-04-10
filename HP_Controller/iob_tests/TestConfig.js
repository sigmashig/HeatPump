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

const HP_STEP = HP_STATUS + ".Script.Step";

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

const HP_TEMP_INSIDE = HP_TEMP + ".TInside";
const HP_TEMP_OUTSIDE = HP_TEMP + ".TOutside";
const HP_TEMP_COMPRESSOR = HP_TEMP + ".TCompressor";
const HP_TEMP_CONDIN = HP_TEMP + ".TCondIn";
const HP_TEMP_CONDVAP = HP_TEMP + ".TCondVap";
const HP_TEMP_GNDIN = HP_TEMP + ".TGndIn";
const HP_TEMP_GNDOUT = HP_TEMP + ".TGndOut";
const HP_TEMP_IN = HP_TEMP + ".TIn";
const HP_TEMP_OUT = HP_TEMP + ".TOut";
const HP_TEMP_TANKIN = HP_TEMP + ".TTankIn";
const HP_TEMP_TANKOUT = HP_TEMP + ".TTankOut";
const HP_TEMP_VAPOUT = HP_TEMP + ".TVapOut";

var HpTestTimer = null;
var HpSteps = [];
var HpStep = 0;
var HpNexStep;
var HpStepSubscription = null;
var HpAlertSubscription = null;


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

    setState(HP_TEMP_INSIDE, 19);
    setState(HP_TEMP_OUTSIDE, -5);
    setState(HP_TEMP_COMPRESSOR, 20);
    setState(HP_TEMP_CONDIN, 30);
    setState(HP_TEMP_CONDVAP, 30);
    setState(HP_TEMP_GNDIN, 7);
    setState(HP_TEMP_GNDOUT, 2);
    setState(HP_TEMP_IN, 20);
    setState(HP_TEMP_OUT, 25);
    setState(HP_TEMP_TANKIN, 22);
    setState(HP_TEMP_TANKOUT, 27);
    setState(HP_TEMP_VAPOUT, 30);

}

function StartTest1() {
    var isReady = getState(HP_ISREADY).val;

    log("isReady=" + isReady);
    if (isReady == '1') {
        SetInitialConfig();
        HpStep = 0;
        ResultProcessing("");
    }
}

function ResultProcessing(result) {

    if (result == "") {
        HpSteps[HpStep]();
    } else {
        if (HpStepSubscription != null) {
            unsubscribe(HpStepSubscription);
            HpStepSubscription = null;
        }

        if (HpAlertSubscription != null) {
            unsubscribe(HpAlertSubscription);
            HpAlertSubscription = null;
        }
        if (HpTestTimer != null) {
            clearTimeout(HpTestTimer);
            HpTestTimer = null;
        }

        if (result != "OK") {
            log("FAILED! Result:" + result, "error");
            Finish();
        } else {
            log("PASSED! Step " + HpStep, "info");
            HpStep++;
            if (HpStep < HpSteps.length) {
                HpSteps[HpStep]();
            }
        }
    }
}


function Finish() {
    log("OK! Test suite finished");
    unsubscribe(HpAlertSubscription);
    SetInitialConfig();
}


/*


*/


function Test_Empty2HeatIdle() {

    log("INFO: Test Empty -> Idle started", "info");
    HpStepSubscription = on({ id: HP_STEP }, function (obj) {
        var newStep = obj.state.val;
        var result;

        unsubscribe(HpStepSubscription); // just one step checked

        if (newStep == '0') {
            result = "OK";
            log("PASSED! Transfer Empty -> Idle", "info");
        } else {
            result = "Wrong Step";
            log("FAIL! New Step is: " + newStep, "error");
        }
        ResultProcessing(result);
    });

    HpTestTimer = setTimeout(function () {
        clearTimeout(HpTestTimer);

        ResultProcessing("Timeout");
    }, 5000);

    setState(HP_HEATCOLD, 1); // Heat
    setState(HP_COMMAND, 0); // No Cmd
}


function Test_Heat_Idle2Initial() {

    log("INFO: Test Idle -> Initial started", "info");
    HpStepSubscription = on({ id: HP_STEP }, function (obj) {
        var newStep = obj.state.val;
        var result;

        unsubscribe(HpStepSubscription); // just one step checked

        if (newStep == 'I') {
            result = "OK";
            log("PASSED! Transfer Idle -> Initial", "info");
        } else {
            result = "Wrong Step";
            log("FAIL! New Step is: " + newStep, "error");
        }
        ResultProcessing(result);
    });

    HpTestTimer = setTimeout(function () {
        clearTimeout(HpTestTimer);

        ResultProcessing("Timeout");
    }, 5000);

    setState(HP_COMMAND, 1); // Run
}


function Test_Heat_Initial2CheckStart() {

    log("INFO: Test Initial -> Check started", "info");
    HpStepSubscription = on({ id: HP_STEP }, function (obj) {
        var newStep = obj.state.val;
        var result;
        unsubscribe(HpStepSubscription); // just one step checked
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

    HpTestTimer = setTimeout(function () {
        clearTimeout(HpTestTimer);

        ResultProcessing("Timeout");
    }, 5000);

    //setState(HP_COMMAND,1); // Run
}

function Test_Heat_CheckStart2StartGnd() {

    log("INFO: Test Check -> Gnd started", "info");
    HpStepSubscription = on({ id: HP_STEP }, function (obj) {
        var newStep = obj.state.val;
        var result;
        unsubscribe(HpStepSubscription); // just one step checked
        if (newStep == 'G') {
            result = "OK";
            log("PASSED! Transfer Check -> Gnd", "info");
        } else {
            result = "Wrong Step";
            log("FAIL! New Step is: " + newStep, "error");
        }
        ResultProcessing(result);
    });


    HpTestTimer = setTimeout(function () {
        clearTimeout(HpTestTimer);

        ResultProcessing("Timeout");
    }, 5000);
    setState(HP_HYST, 2);
    setState(HP_DESIREDTEMP, 19);
    setState(HP_TEMP_INSIDE, 16); // 19 - 2 = 17
}

function Test_Heat_StartGnd2Heat() {

    log("INFO: Test Gnd -> Heat started", "info");
    HpStepSubscription = on({ id: HP_STEP }, function (obj) {
        var newStep = obj.state.val;
        var result;
        unsubscribe(HpStepSubscription); // just one step checked
        //         log("PumpGND=" + HP_PUMP_GND + "; Pump TankIn=" + HP_PUMP_TANKIN);
        if (getState(HP_PUMP_GND).val == 1 && getState(HP_PUMP_TANKIN).val == 1) {
            log("All pumps started", "info");
            if (newStep == 'H') {
                result = "OK";
                log("PASSED! Transfer Gnd -> Heat", "info");
            } else {
                result = "Wrong Step";
                log("FAIL! New Step is: " + newStep, "error");
            }
        } else {
            result = "Hardware Error";
            log("Some pumps do not started: GND=" + getState(HP_PUMP_GND).val + " TankIn=" + getState(HP_PUMP_TANKIN).val);
        }
        ResultProcessing(result);
    });


    HpTestTimer = setTimeout(function () {
        clearTimeout(HpTestTimer);
        ResultProcessing("Timeout");
    }, 5000);

    //    setState(HP_TEMP_INSIDE,16); // 19 - 2 = 17
}

function Test_Heat_Heat2CheckStop() {

    log("INFO: Test Heat -> CheckStop  started", "info");
    HpStepSubscription = on({ id: HP_STEP }, function (obj) {
        var newStep = obj.state.val;
        var result;
        unsubscribe(HpStepSubscription); // just one step checked
        if (getState(HP_COMPRESSOR).val == 1) {
            log("Compressor started", "info");
            if (newStep == 'h') {
                result = "OK";
                log("PASSED! Transfer Heat -> CheckStop", "info");
            } else {
                result = "Wrong Step";
                log("FAIL! New Step is: " + newStep, "error");
            }
        } else {
            result = "Hardware Error";
            log("Compressor do not started: GND=" + getState(HP_COMPRESSOR).val);

        }
        ResultProcessing(result);
    });


    HpTestTimer = setTimeout(function () {
        clearTimeout(HpTestTimer);
        ResultProcessing("Timeout");
    }, 5000);

    //    setState(HP_HYST, 2);
    //    setState(HP_DESIREDTEMP, 19);
    //    setState(HP_TEMP_INSIDE, 22); // 19 + 2 = 21
}

function Test_Heat_CheckStop2StopCompressor() {

    log("INFO: Test CheckStop ->  Stop compressor started", "info");
    HpStepSubscription = on({ id: HP_STEP }, function (obj) {
        var newStep = obj.state.val;
        var result;
        unsubscribe(HpStepSubscription); // just one step checked
        if (newStep == 'C') {
            result = "OK";
            log("PASSED! Transfer CheckStop ->  Stop compressor", "info");
        } else {
            result = "Wrong Step";
            log("FAIL! New Step is: " + newStep, "error");
        }
        ResultProcessing(result);
    });


    HpTestTimer = setTimeout(function () {
        clearTimeout(HpTestTimer);
        ResultProcessing("Timeout");
    }, 5000);

    setState(HP_HYST, 2);
    setState(HP_DESIREDTEMP, 19);
    setState(HP_TEMP_INSIDE, 22); // 19 + 2 = 21
}


function Test_Heat_StopCompressor2StopHeating() {

    log("INFO: Test Stop Compressor -> Stop Heating started", "info");
    HpStepSubscription = on({ id: HP_STEP }, function (obj) {
        var newStep = obj.state.val;
        var result;
        unsubscribe(HpStepSubscription); // just one step checked
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


    HpTestTimer = setTimeout(function () {
        clearTimeout(HpTestTimer);
        ResultProcessing("Timeout");
    }, 5000);

    //    setState(HP_TEMP_INSIDE,16); // 19 - 2 = 17
}

function Test_Heat_StopHeating2CheckStart() {

    log("INFO: Test Stop Heating -> Check Start started", "info");
    HpStepSubscription = on({ id: HP_STEP }, function (obj) {
        var newStep = obj.state.val;
        var result;
        unsubscribe(HpStepSubscription); // just one step checked
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


    HpTestTimer = setTimeout(function () {
        clearTimeout(HpTestTimer);
        ResultProcessing("Timeout");
    }, 5000);

    //    setState(HP_TEMP_INSIDE,16); // 19 - 2 = 17
}



