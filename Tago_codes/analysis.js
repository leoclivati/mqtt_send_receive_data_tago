const Analysis = require('tago/analysis');
const Services = require('tago/services');

function myanalysis(context, scope) {
  if (!scope.length) return context.log('no data');
  
  const temperature = scope[1]['value'];
  const humidity = scope[2]['value'];
  const luminosity = scope[3]['value'];
  let ac_state;
  let hum_state;
  let hea_state;
  let lig_state;

  if(temperature > 27) ac_state = 1;
  else ac_state = 0;
  if(humidity < 70) hum_state = 1;
  else hum_state = 0;
  if(luminosity < 1000) lig_state = 1;
  else lig_state = 0;
  if(temperature < 19) hea_state = 1;
  else hea_state = 0;

  const states = {
      ac: ac_state,
      humidifier: hum_state,
      heater:hea_state,
      lights:lig_state,
  };
  const options = {
      retain: false,
      qos: 0,
  };

  const MQTT = new Services(context.token).MQTT;

  MQTT.publish('device/states', JSON.stringify(states), "644e89a7043e750008d752ac", options).then(context.log).catch(context.log);
}

module.exports = new Analysis(myanalysis, 'YOUR ANALYSIS TOKEN HERE');