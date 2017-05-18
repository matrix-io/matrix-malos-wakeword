
# MATRIX Creator MALOS-Wakeword

Wakeword voice service for MALOS. The last version support:

* setup wakeword
* define lenguaje models path (pre-generated)
* define lenguaje dictionary path (pre-generated)
* define mic number (0-8)
* enable/disable pocketsphinx verbose debugging 
* send and override configuration (hot-plug)
* disable voice recognition service (stop pshinx main thread)


## Installation

### Raspbian Dependencies 

Before, please install MALOS on your RaspberryPi3 and perform device reboot. For more details: [Getting Started Guide](https://github.com/matrix-io/matrix-creator-quickstart/wiki/2.-Getting-Started)

``` 
echo "deb http://packages.matrix.one/matrix-creator/ ./" | sudo tee --append /etc/apt/sources.list
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install matrix-creator-init matrix-creator-malos cmake g++ git libzmq3-dev --no-install-recommends
reboot
```

Add some dependencies and pocketsphinx package:

``` 
echo "deb http://unstable-packages.matrix.one/ stable main" | sudo tee -a /etc/apt/sources.list
sudo apt-get update
sudo apt-get install matrix-creator-malos-wakeword --no-install-recommends
```

Nodejs and npm on RaspberryPi:

``` 
curl -sL https://deb.nodesource.com/setup_6.x | sudo bash -
sudo apt-get install nodejs
```

## Run sample DEMO

Before, copy on your RaspberryPi lengueaje models and dictionary for run the DEMO or generated
them (see section, generating models)

``` javascript
cd /home/pi
git clone --recursive https://github.com/matrix-io/matrix-malos-wakeword.git
cp -r matrix-malos-wakeword/assets .
```


Run nodejs example and say some voice commands: `mia ring red`, `mia ring
orange`, `mia ring clear` for example:

``` javascript
cd matrix-malos-wakeword
git submodule init && git submodule update
cd src/js_test
node test_wakeword.js
```

## Documentation

The driver follows the [MALOS protocol](https://github.com/matrix-io/matrix-creator-malos/blob/master/README.md#protocol).


### 0MQ Port
```
20049
```
### Protocol buffers

``` javascript
message WakeWordParams {
  // Wake Word
  string wake_word = 1;

  // Mic channel
  enum MicChannel {
    channel0 = 0;
    channel1 = 1;
    channel2 = 2;
    channel3 = 3;
    channel4 = 4;
    channel5 = 5;
    channel6 = 6;
    channel7 = 7;
    channel8 = 8;
  }

  MicChannel channel = 2;

  // lenguaje model path from lmtool or similar alternative:
  // http://www.speech.cs.cmu.edu/tools/lmtool-new.html
  string lm_path = 3;

  // dictionary path from lmtool 
  string dic_path = 4;

  // enable pocketsphinx verbose mode
  bool enable_verbose = 5;

  // stop recognition service
  bool stop_recognition = 6;
}

```
The message is defined in [driver.proto](https://github.com/matrix-io/protocol-buffers/blob/master/malos/driver.proto).

### JavaScript example

Enhanced description of the [sample source code](src/js_test/test_wakeword.js).

First, define the address and port of the MATRIX Creator. In this case we make it be `127.0.0.1`
because we are connecting from the local host but it needs to be different if we
connect from another computer.

``` javascript
var creator_ip = '127.0.0.1'
var creator_wakeword_base_port = 60001;
```

#### config and start Wakeup service

``` javascript
/**************************************
 * start/stop service functions
 **************************************/

function startWakeUpRecognition(){
  console.log('<== config wakeword recognition..')
  var wakeword_config = new matrixMalosBuilder.WakeWordParams;
  wakeword_config.set_wake_word("MIA");
  wakeword_config.set_lm_path("/home/pi/assets/9854.lm");
  wakeword_config.set_dic_path("/home/pi/assets/9854.dic");
  wakeword_config.set_channel(matrixMalosBuilder.WakeWordParams.MicChannel.channel0);
  wakeword_config.set_enable_verbose(false)
  sendConfigProto(wakeword_config);
}
```

#### stop Wakeup service
``` javascript

```

#### stop Wakeup service
``` javascript
function stopWakeUpRecognition(){
  console.log('<== stop wakeword recognition..')
  var wakeword_config = new matrixMalosBuilder.WakeWordParams;
  wakeword_config.set_stop_recognition(true)
  sendConfigProto(wakeword_config);
}
```

#### register Wakeup callbacks (voice commands)
``` javascript
var updateSocket = zmq.socket('sub')
updateSocket.connect('tcp://' + creator_ip + ':' + (creator_wakeword_base_port + 3))
updateSocket.subscribe('')

updateSocket.on('message', function(wakeword_buffer) {
  var wakeWordData = new matrixMalosBuilder.WakeWordParams.decode(wakeword_buffer);
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
      default:
        text = "==> Not handled voice command";
    }
});

```








