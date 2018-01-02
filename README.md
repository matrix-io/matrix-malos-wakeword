
# MATRIX Creator MALOS-Wakeword

Wakeword voice service for MALOS. The last version support:

* setup wakeword
* define language models path (pre-generated)
* define language dictionary path (pre-generated)
* define mic number (0-8)
* enable/disable pocketsphinx verbose debugging 
* send and override configuration (hot-plug)
* disable voice recognition service (stop pshinx main thread)
* optional: support for null paths and models (not recommend, bad performance)


## Installation

### Raspbian Dependencies 

Before, please install **MALOS** on your `RaspberryPi3` and perform device reboot: 

``` bash 
curl https://apt.matrix.one/doc/apt-key.gpg | sudo apt-key add -
echo "deb https://apt.matrix.one/raspbian $(lsb_release -sc) main" | sudo tee /etc/apt/sources.list.d/matrixlabs.list
sudo apt-get update
sudo apt-get upgrade
sudo apt install matrixio-malos
reboot
```

Install `wakeword` package and dependencies:

``` nodejs 
sudo apt-get install matrixio-malos-wakeword --no-install-recommends
sudo reboot
```

**Nodejs** and npm on RaspberryPi:

``` 
curl -sL https://deb.nodesource.com/setup_6.x | sudo bash -
sudo apt-get install nodejs
```

## Run sample DEMO

First, copy on your RaspberryPi language models and dictionary for run the DEMO or generated
them (see section: Custom language and phrases for recognition)

``` bash
cd /home/pi
git clone --recursive https://github.com/matrix-io/matrix-malos-wakeword.git
cp -r matrix-malos-wakeword/assets .
```

Run nodejs example and say some voice commands: `mia ring red`, `mia ring
orange`, `mia ring clear` for example:

``` nodejs
cd matrix-malos-wakeword/src/js_test
npm install
node test_wakeword.js
```

[explanation](#javascript-example)

## Documentation

The driver follows the [MALOS protocol](https://github.com/matrix-io/matrix-creator-malos/blob/master/README.md#protocol).


### 0MQ Port
```
60001
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

  // language model path from lmtool or similar alternative:
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

#### Config and start wakeupword service

``` javascript
function startWakeUpRecognition(){
  console.log('<== config wakeword recognition..')
  var wakeword_config = matrix_io.malos.v1.io.WakeWordParams.create({
    wakeWord: 'MIA',
    lmPath: LM_PATH,
    dicPath: DIC_PATH,
    channel: matrix_io.malos.v1.io.WakeWordParams.MicChannel.channel8,
    enableVerbose: false
  });

  sendConfigProto(wakeword_config);
}
```

#### Stop wakeupword service

``` javascript
function stopWakeUpRecognition(){
  console.log('<== stop wakeword recognition..')
  var wakeword_config = matrix_io.malos.v1.io.WakeWordParams.create({stopRecognition: true});
  sendConfigProto(wakeword_config);
}
```

#### Register wakeupword callbacks (voice commands)

``` javascript
var updateSocket = zmq.socket('sub')
updateSocket.connect('tcp://' + creator_ip + ':' + (creator_wakeword_base_port + 3))
updateSocket.subscribe('')

updateSocket.on('message', function(wakeword_buffer) {
  var wakeWordData = matrix_io.malos.v1.io.WakeWordParams.decode(wakeword_buffer);
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
```

## Custom language and phrases for recognition 

1. Make a text plane like this: 

  ``` nodejs
  matrix everloop
  matrix clear
  matrix stop
  matrix ipaddress
  matrix ring red
  matrix ring green
  matrix game time
  matrix five minutes
  matrix ten seconds
  matrix close door
  ```

2. Upload this file to [Sphinx Knowledge Base Tool](http://www.speech.cs.cmu.edu/tools/lmtool-new.html) and compile knowledge base.

3. Dowload *TARXXXXX.tgz* and upgrade assets directory.

4. **NOTE**: change wakeword and paths on config/start wakeword service method


## Build Debian package from source (optional)

Update source and submodules and install headers and tools:

``` bash
cd matrix-malos-wakeword
sudo apt-get install devscripts dh-make --no-install-recommends libmatrixio-protos-dev
```

Build Debian package on RaspberryPi:

``` bash
debuild -us -uc -j4
```

Install and start wakeword service:

``` bash
cd ..
sudo dpkg -i ../matrixio-malos-wakeword_xxx_armhf.deb
sudo service matrixio-malos-wakeword start
```

