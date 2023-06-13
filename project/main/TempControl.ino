struct TempControlData
{
    float internalTempC;
    float externalTempC;
    long timestamp_millis;
    bool control;
};

/*
    Constantes derivadas dos requisitos
*/

//freq medida e armazenamento
const long internalTemperatureMeasurementRateMillis = 5000;
const float internalTemperatureGoal = 15.0;
const float internalTemperatureTolerance = 2.0;

bool onOff