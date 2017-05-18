
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
-------------------------

### Raspbian Dependencies 

Before, please install MALOS on your RaspberryPi3 and perform device reboot. For more details: [Getting Started Guide](https://github.com/matrix-io/matrix-creator-quickstart/wiki/2.-Getting-Started)

``` javascript
echo "deb http://packages.matrix.one/matrix-creator/ ./" | sudo tee --append /etc/apt/sources.list
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install matrix-creator-init matrix-creator-malos cmake g++ git libzmq3-dev --no-install-recommends
reboot
```

Add some dependencies and pocketsphinx package:

``` javascript
echo "deb http://unstable-packages.matrix.one/ stable main" | sudo tee -a /etc/apt/sources.list
sudo apt-get update
sudo apt-get install matrix-creator-malos-wakeword --no-install-recommends
```

Nodejs and npm on RaspberryPi:

``` javascript
curl -sL https://deb.nodesource.com/setup_6.x | sudo bash -
sudo apt-get install nodejs
```

## Usage

Before, copy on your RaspberryPi lengueaje models and dictionary for run the DEMO or generated
them (see section, generating models)

``` javascript
cd /home/pi
git clone --recursive https://github.com/matrix-io/matrix-malos-wakeword.git
cp -r matrix-malos-wakeword/assets .
```

### Run sample DEMO

Run nodejs example and say it some voice commands: `mia ring red`, `mia ring
orange`, mia ring clear` for example:

``` javascript
cd matrix-malos-wakeword
git submodule init && git submodule update
cd src/js_test
node test_wakeword.js
```


