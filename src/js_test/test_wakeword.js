
// This is how we connect to the creator. IP and port.
// The IP is the IP I'm using and you need to edit it.
// By default, MALOS has its 0MQ ports open to the world.

// Every device is identified by a base port. Then the mapping works
// as follows:
// BasePort     => Configuration port. Used to config the device.
// BasePort + 1 => Keepalive port. Send pings to this port.
// BasePort + 2 => Error port. Receive errros from device.
// BasePort + 3 => Data port. Receive data from device.

var creator_ip = '127.0.0.1'
var creator_wakeword_base_port = 60001
var creator_everloop_base_port = 20013 + 8 // port for Everloop driver.
var Root = require("protobufjs").Root
var zmq = require('zmq')

const PROTO_PATH = '../../protocol-buffers/' // keep the trailing slash
const LM_PATH = '/home/pi/assets/9854.lm'
const DIC_PATH = '/home/pi/assets/9854.dic'

// ******** Load proto messages ************
var root = new Root()
root.loadSync([
  PROTO_PATH + 'matrix_io/malos/v1/driver.proto',
  PROTO_PATH + 'matrix_io/malos/v1/io.proto',
  PROTO_PATH + 'matrix_io/malos/v1/comm.proto',
  PROTO_PATH + 'matrix_io/malos/v1/sense.proto'
])

var DriverConfigProto = root.lookup('matrix_io.malos.v1.driver.DriverConfig')
var EverloopImageProto = root.lookup('matrix_io.malos.v1.io.EverloopImage')
var LedValueProto = root.lookup('matrix_io.malos.v1.io.LedValue')
var WakeWordParamsProto = root.lookup('matrix_io.malos.v1.io.WakeWordParams')

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
  var wakeword_config = WakeWordParamsProto.create({
    wakeWord: 'MIA',
    lmPath: LM_PATH,
    dicPath: DIC_PATH,
    channel: WakeWordParamsProto.MicChannel.channel8,
    enableVerbose: false
  });

  sendConfigProto(wakeword_config);
}

function stopWakeUpRecognition(){
  console.log('<== stop wakeword recognition..')
  var wakeword_config = WakeWordParamsProto.create({stopRecognition: true});
  sendConfigProto(wakeword_config);
}

/**************************************
 * Register wakeword callbacks
 **************************************/

var updateSocket = zmq.socket('sub')
updateSocket.connect('tcp://' + creator_ip + ':' + (creator_wakeword_base_port + 3))
updateSocket.subscribe('')

updateSocket.on('message', function(wakeword_buffer) {
  var wakeWordData = WakeWordParamsProto.decode(wakeword_buffer);
  console.log('==> WakeWord Reached:', wakeWordData.wakeWord)
    
    switch(wakeWordData.wakeWord) {
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
    var image = EverloopImageProto.create();
    for (var j = 0; j < 35; ++j) {
      var ledValue = LedValueProto.create({
        red: Math.round(r*i),
        green: Math.round(g*i),
        blue: Math.round(b*i),
        white: Math.round(w*i)
      });
      image.led.push(ledValue)
    }
    var config = DriverConfigProto.create({
      image: image
    })
    const serialized = JSON.stringify(DriverConfigProto.toObject(config))
    console.log('==> Everloop event', serialized)
    ledsConfigSocket.send(DriverConfigProto.encode(config).finish());
}

/**************************************
 * sendConfigProto: build Proto message 
 **************************************/

function sendConfigProto(cfg){
  var config = DriverConfigProto.create({ wakeword: cfg })
  const serialized = JSON.stringify(DriverConfigProto.toObject(config))
  console.log("==> sending conf ", serialized)
  configSocket.send(DriverConfigProto.encode(config).finish())
}

/**********************************************
 ****************** MAIN **********************
 **********************************************/

startWakeUpRecognition();


