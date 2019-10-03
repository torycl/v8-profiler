var binding = require('./build/Release/profiler.node');
var Stream = require('stream').Stream;

function CpuProfile() {}

CpuProfile.prototype.getHeader = function() {
  return {
    typeId: this.typeId,
    uid: this.uid,
    title: this.title
  }
}

CpuProfile.prototype.export = function(dataReceiver) {
  dataReceiver = dataReceiver || new ExportStream();

  var toStream = dataReceiver instanceof Stream;
  var error, result;

  try {
    result = JSON.stringify(this);
  } catch (err) {
    error = err;
  }

  process.nextTick(function() {
    if (toStream) {
      if (error) {
        dataReceiver.emit('error', error);
      }

      dataReceiver.end(result);
    } else {
      dataReceiver(error, result);
    }
  });

  return toStream ? dataReceiver : undefined;
};

var startTime, endTime;
var activeProfiles = [];

var profiler = {
  /*CPU PROFILER API*/

  get profiles() { return binding.cpu.profiles; },

  startProfiling: function(name, recsamples) {
    if (activeProfiles.length == 0 && typeof process._startProfilerIdleNotifier == "function")
      process._startProfilerIdleNotifier();

    if (typeof name == 'boolean') {
      recsamples = name;
      name = '';
    }

    recsamples = recsamples === undefined ? true : Boolean(recsamples);
    name = '' + name;

    if (activeProfiles.indexOf(name) < 0)
      activeProfiles.push(name)

    startTime = Date.now();
    binding.cpu.startProfiling(name, recsamples);
  },

  stopProfiling: function(name) {
    var index = activeProfiles.indexOf(name);
    if (name && index < 0)
      return;

    var profile = binding.cpu.stopProfiling(name);
    endTime = Date.now();
    profile.__proto__ = CpuProfile.prototype;
    if (!profile.startTime) profile.startTime = startTime;
    if (!profile.endTime) profile.endTime = endTime;

    if (name)
      activeProfiles.splice(index, 1);
    else
      activeProfiles.length = activeProfiles.length - 1;

    if (activeProfiles.length == 0 && typeof process._stopProfilerIdleNotifier == "function")
      process._stopProfilerIdleNotifier();

    return profile;
  },

  setSamplingInterval: function(num) {
    if (activeProfiles.length) {
      throw new Error('setSamplingInterval must be called when there are no profiles being recorded.');
    }

    num = parseInt(num, 10) || 1000;
    binding.cpu.setSamplingInterval(num);
  },

  deleteAllProfiles: function() {
    Object.keys(binding.cpu.profiles).forEach(function(key) {
      binding.cpu.profiles[key].delete();
    });
  }
};

module.exports = profiler;
process.profiler = profiler;
