
# Pre-Requisites
```
echo "deb http://packages.matrix.one/matrix-creator/ ./" | sudo tee --append /etc/apt/sources.list;
sudo apt-get update;
sudo apt-get upgrade;
sudo apt-get install libzmq3-dev cmake g++ git;
```

### Install
```
sudo apt-get install matrix-creator-init matrix-creator-malos matrix-malos-wakeword
sudo reboot
```

**Note:** At this point, on next start, `malos` and `malos-wakeword` will be running as a service.


### Examples
**Note:** pre-requisite is NodeJS. Don't use the one shipped with Raspbian because it's a bit old. If you don't have it, please check a recipe included below.
```
git clone https://github.com/matrix-io/matrix-malos-wakeword.git && cd matrix-malos-wakeword
git submodule init && git submodule update
cd src/js_test


// uv index, uv range
node test_wakeword.js
```
-------------------------

### NodeJS Dependency

For instance (in the Raspberry):

```
# Install npm (doesn't really matter what version, apt-get node is v0.10...)
sudo apt-get install npm

# n is a node version manager
sudo npm install -g n

# node 6.5 is the latest target node version, also installs new npm
n 6.5

# check version
node -v
```


