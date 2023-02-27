// Wrap the script in a method, so that you can do "return false;" in case of an error or stop request
function publish()
{
    mqttspy.publish("HeatPump/Board_07/Config/Command", 0, 0, false);
    mqttspy.publish("HeatPump/Board_07/Config/HeatCold", 1, 0, false);
    mqttspy.publish("HeatPump/Board_07/Config/Hysteresis", 1, 0, false);
    mqttspy.publish("HeatPump/Board_07/Config/Mode", 0, 0, false);
    mqttspy.publish("HeatPump/Board_07/Config/WeekMode", 0, 0, false);
    mqttspy.publish("HeatPump/Board_07/Config/ManualTemp", 25.0, 0, false);
    mqttspy.publish("HeatPump/Board_07/Config/Simulator", 1, 0, false);
    mqttspy.publish("HeatPump/Board_07/Config/TimeZone", "EET", 0, false);
    mqttspy.publish("HeatPump/Board_07/Config/HystGround", 5, 0, false);
    mqttspy.publish("HeatPump/Board_07/Config/HystHpRoom", 5, 0, false);


    mqttspy.publish("HeatPump/Board_07/Schedule/WorkDay/Set01", "{'time':'23:00','temp':'30.0'}", 0, false);
    mqttspy.publish("HeatPump/Board_07/Schedule/WorkDay/Set02", "{'time':'00:00','temp':'35.0'}", 0, false);
    mqttspy.publish("HeatPump/Board_07/Schedule/WorkDay/Set03", "{'time':'07:00','temp':'22.0'}", 0, false);
    mqttspy.publish("HeatPump/Board_07/Schedule/WorkDay/Set04", "{'time':'13:00','temp':'28.0'}", 0, false);
    mqttspy.publish("HeatPump/Board_07/Schedule/WorkDay/Set05", "{'time':'15:00','temp':'22.0'}", 0, false);
    mqttspy.publish("HeatPump/Board_07/Schedule/WorkDay/Set06", "{'time':'18:00','temp':'25.0'}", 0, false);

    mqttspy.publish("HeatPump/Board_07/Schedule/WeekEnd/Set01", "{'time':'23:00','temp':'30.0'}", 0, false);
    mqttspy.publish("HeatPump/Board_07/Schedule/WeekEnd/Set02", "{'time':'00:00','temp':'35.0'}", 0, false);
    mqttspy.publish("HeatPump/Board_07/Schedule/WeekEnd/Set03", "{'time':'07:00','temp':'22.0'}", 0, false);
    mqttspy.publish("HeatPump/Board_07/Schedule/WeekEnd/Set04", "{'time':'13:00','temp':'28.0'}", 0, false);
    mqttspy.publish("HeatPump/Board_07/Schedule/WeekEnd/Set05", "{'time':'15:00','temp':'22.0'}", 0, false);
    mqttspy.publish("HeatPump/Board_07/Schedule/WeekEnd/Set06", "{'time':'18:00','temp':'25.0'}", 0, false);

    mqttspy.publish("HeatPump/Board_07/Equipment/Relay/MotorGnd", "{'pin':0, 'lhOn':0, 'minTimeOff' : 60}", 0, false);
    mqttspy.publish("HeatPump/Board_07/Equipment/Relay/Compressor", "{'pin':0, 'lhOn':0, 'minTimeOff' : 180, 'maxtimeOn' : 1800}", 0, false);
    mqttspy.publish("HeatPump/Board_07/Equipment/Relay/PumpTankIn", "{'pin':0, 'lhOn':0}", 0, false);
    mqttspy.publish("HeatPump/Board_07/Equipment/Relay/PumpTankOut", "{'pin':0, 'lhOn':0}", 0, false);
    mqttspy.publish("HeatPump/Board_07/Equipment/Relay/PumpFloor1", "{'pin':0, 'lhOn':0}", 0, false);
    mqttspy.publish("HeatPump/Board_07/Equipment/Relay/PumpFloor2", "{'pin':0, 'lhOn':0}", 0, false);

    mqttspy.publish("HeatPump/Board_07/Status/Relay/MotorGnd", "0", 0, false);
    mqttspy.publish("HeatPump/Board_07/Status/Relay/Compressor", "0", 0, false);
    mqttspy.publish("HeatPump/Board_07/Status/Relay/PumpTankIn", "0", 0, false);
    mqttspy.publish("HeatPump/Board_07/Status/Relay/PumpTankOut", "0", 0, false);
    mqttspy.publish("HeatPump/Board_07/Status/Relay/PumpFloor1", "0", 0, false);
    mqttspy.publish("HeatPump/Board_07/Status/Relay/PumpFloor2", "0", 0, false);


    mqttspy.publish("HeatPump/Board_07/Equipment/Temperature/Bus", "0", 0, false);

    mqttspy.publish("HeatPump/Board_07/Equipment/Temperature/TGndIn", "{'address':'0000000000000000', 'errorLow':'-30', 'warningLow':-20, 'warningHigh':100, 'errorHigh':130}", 0, false);
    mqttspy.publish("HeatPump/Board_07/Equipment/Temperature/TGndOut", "{'address':'0000000000000000',  'errorLow':'-30', 'warningLow':-20, 'warningHigh':100, 'errorHigh':130}", 0, false);
    mqttspy.publish("HeatPump/Board_07/Equipment/Temperature/TCompressor", "{'address':'0000000000000000',  'errorLow':'-30', 'warningLow':-20, 'warningHigh':100, 'errorHigh':120}", 0, false);
    mqttspy.publish("HeatPump/Board_07/Equipment/Temperature/TVapOut", "{'address':'0000000000000000',  'errorLow':'-30', 'warningLow':-20, 'warningHigh':100, 'errorHigh':120}", 0, false);
    mqttspy.publish("HeatPump/Board_07/Equipment/Temperature/TCondIn", "{'address':'0000000000000000',  'errorLow':'-30', 'warningLow':-20, 'warningHigh':100, 'errorHigh':120}", 0, false);
    mqttspy.publish("HeatPump/Board_07/Equipment/Temperature/TCondVap", "{'address':'0000000000000000',  'errorLow':'-30', 'warningLow':-20, 'warningHigh':100, 'errorHigh':120}", 0, false);
    mqttspy.publish("HeatPump/Board_07/Equipment/Temperature/TOut", "{'address':'0000000000000000',  'errorLow':'-30', 'warningLow':-20, 'warningHigh':100, 'errorHigh':120}", 0, false);
    mqttspy.publish("HeatPump/Board_07/Equipment/Temperature/TIn", "{'address':'0000000000000000',  'errorLow':'-30', 'warningLow':-20, 'warningHigh':100, 'errorHigh':120}", 0, false);
    mqttspy.publish("HeatPump/Board_07/Equipment/Temperature/TTankOut", "{'address':'0000000000000000',  'errorLow':'-30', 'warningLow':-20, 'warningHigh':100, 'errorHigh':120}", 0, false);
    mqttspy.publish("HeatPump/Board_07/Equipment/Temperature/TTankIn", "{'address':'0000000000000000',  'errorLow':'-30', 'warningLow':-20, 'warningHigh':100, 'errorHigh':120}", 0, false);
    mqttspy.publish("HeatPump/Board_07/Equipment/Temperature/TInside", "{'address':'0000000000000000',  'errorLow':'-30', 'warningLow':-20, 'warningHigh':100, 'errorHigh':120}", 0, false);
    mqttspy.publish("HeatPump/Board_07/Equipment/Temperature/TOutside", "{'address':'0000000000000000',  'errorLow':'-30', 'warningLow':-20, 'warningHigh':100, 'errorHigh':120}", 0, false);
/*
    mqttspy.publish("HeatPump/Board_07/Status/Temperature/TGndIn", "7.0", 0, false);
    mqttspy.publish("HeatPump/Board_07/Status/Temperature/TGndOut", "4.0", 0, false);
    mqttspy.publish("HeatPump/Board_07/Status/Temperature/TCompressor", "100.5", 0, false);
    mqttspy.publish("HeatPump/Board_07/Status/Temperature/TVapOut", "5", 0, false);
    mqttspy.publish("HeatPump/Board_07/Status/Temperature/TCondIn", "80", 0, false);
    mqttspy.publish("HeatPump/Board_07/Status/Temperature/TCondVap", "60", 0, false);
    mqttspy.publish("HeatPump/Board_07/Status/Temperature/TOut", "30", 0, false);
    mqttspy.publish("HeatPump/Board_07/Status/Temperature/TIn", "20", 0, false);
    mqttspy.publish("HeatPump/Board_07/Status/Temperature/TTankOut", "28", 0, false);
    mqttspy.publish("HeatPump/Board_07/Status/Temperature/TTankIn", "20", 0, false);
    mqttspy.publish("HeatPump/Board_07/Status/Temperature/TInside", "20", 0, false);
    mqttspy.publish("HeatPump/Board_07/Status/Temperature/TOutside", "-5", 0, false);


 
    mqttspy.publish("HeatPump/Board_07/Equipment/Contactor/PressureSwitch", "{'pin':0, 'lhOn':0}", 0, false);
    mqttspy.publish("HeatPump/Board_07/Equipment/Contactor/VoltageSwitch", "{'pin':0, 'lhOn':0}", 0, false);

    mqttspy.publish("HeatPump/Board_07/Status/Contactor/PressureSwitch", "0", 0, false);
    mqttspy.publish("HeatPump/Board_07/Status/Contactor/VoltageSwitch", "0", 0, false);

    mqttspy.publish("HeatPump/Board_07/Equipment/PowerMeter/PMA", "{'SerialRX':63,'SerialTX':54, 'version':3}", 0, false);
    mqttspy.publish("HeatPump/Board_07/Equipment/PowerMeter/PMB", "{'SerialRX':64,'SerialTX':55, 'version':3}", 0, false);
    mqttspy.publish("HeatPump/Board_07/Equipment/PowerMeter/PMC", "{'SerialRX':65,'SerialTX':56, 'version':3}", 0, false);
*/
        // This means all OK, script has completed without any issues and as expected
        return true;
}

publish();