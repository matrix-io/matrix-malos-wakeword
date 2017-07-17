
// This is how we connect to the creator. IP and port.
// The IP is the IP I'm using and you need to edit it.
// By default, MALOS has its 0MQ ports open to the world.

// Every device is identified by a base port. Then the mapping works
// as follows:
// BasePort     => Configuration port. Used to config the device.
// BasePort + 1 => Keepalive port. Send pings to this port.
// BasePort + 2 => Error port. Receive errros from device.
// BasePort + 3 => Data port. Receive data from device.

var creator_ip = '127.0.0.1';
var creator_wakeword_base_port = 60001;
var creator_everloop_base_port = 20013 + 8 // port for Everloop driver.
var protoBuf = require("protobufjs");
var zmq = require('zmq');

const PROTO_PATH = '../../protocol-buffers/'

//  Load proto files
var driverProtoBuilder = protoBuf.loadProtoFile({
  root: PROTO_PATH, 
  file: 'matrix_io/malos/v1/driver.proto'
})
var ioProtoBuilder = protoBuf.loadProtoFile({
  root: PROTO_PATH, 
  file: 'matrix_io/malos/v1/io.proto'
})

var matrix = {
  malos: {
    v1: {
      driver: driverProtoBuilder.build("matrix_io.malos.v1.driver"),
      io: ioProtoBuilder.build("matrix_io.malos.v1.io")
    }
  }
}

var configSocket = zmq.socket('push')
configSocket.connect('tcp://' + creator_ip + ':' + creator_wakeword_base_port /* config */)

// ********** Start error management.
var errorSocket = zmq.socket('sub')
errorSocket.connect('tcp://' + creator_ip + ':' + (creator_wakeword_base_port + 2))
errorSocket.subscribe('')
errorSocket.on('message', function(error_message) {
  process.stdout.write('Received Wakeword error: ' + error_message.toString('utf8') + "\n")
});
// ********** End error management.

/**************************************
 * start/stop service functions
 **************************************/

function startWakeUpRecognition(){
  console.log('<== config wakeword recognition..')
  var wakeword_config = new matrix.malos.v1.io.WakeWordParams;
  wakeword_config.set_channel(matrix.malos.v1.io.WakeWordParams.MicChannel.channel8);
  wakeword_config.set_enable_verbose(false)
  sendConfigProto(wakeword_config);
}

function stopWakeUpRecognition(){
  console.log('<== stop wakeword recognition..')
  var wakeword_config = new matrix.malos.v1.io.WakeWordParams;
  wakeword_config.set_stop_recognition(true)
  sendConfigProto(wakeword_config);
}

/**************************************
 * Register wakeword callbacks
 **************************************/

var updateSocket = zmq.socket('sub')
updateSocket.connect('tcp://' + creator_ip + ':' + (creator_wakeword_base_port + 3))
updateSocket.subscribe('')

updateSocket.on('message', function(wakeword_buffer) {
  var wakeWordData = new matrix.malos.v1.io.WakeWordParams.decode(wakeword_buffer);
  console.log('==> WakeWord Reached:',wakeWordData.wake_word)
    
    switch(wakeWordData.wake_word) {
      case "MIA RING RED":
        setEverloop(255, 0, 25, 0, 0.05)
        break;
      case "MIA RING BLUE":
        setEverloop(0, 25, 255, 0, 0.05) 
        break;
      case "MIA RING GREEN":
        setEverloop(0, 255, 100, 0, 0.05) 
        break;
      case "MIA RING ORANGE":
        setEverloop(255, 77, 0, 0, 0.05) 
        break;
      case "MIA RING CLEAR":
        setEverloop(0, 0, 0, 0, 0) 
        break;
    }
});

/**************************************
 * Everloop Ring LEDs handler
 **************************************/

var ledsConfigSocket = zmq.socket('push')
ledsConfigSocket.connect('tcp://' + creator_ip + ':' + creator_everloop_base_port /* config */)

function setEverloop(r, g, b, w, i) {
    var config = new matrix.malos.v1.driver.DriverConfig
    config.image = new matrix.malos.v1.io.EverloopImage
    for (var j = 0; j < 35; ++j) {
      var ledValue = new matrix.malos.v1.io.LedValue;
      ledValue.setRed(Math.round(r*i));
      ledValue.setGreen(Math.round(g*i));
      ledValue.setBlue(Math.round(b*i));
      ledValue.setWhite(Math.round(w*i));
      config.image.led.push(ledValue)
    }
    ledsConfigSocket.send(config.encode().toBuffer());
}

/**************************************
 * sendConfigProto: build Proto message 
 **************************************/

function sendConfigProto(cfg){
  var config = new matrix.malos.v1.driver.DriverConfig
  config.set_wakeword(cfg)
  configSocket.send(config.encode().toBuffer())
}

/**********************************************
 ****************** MAIN **********************
 **********************************************/

startWakeUpRecognition();


