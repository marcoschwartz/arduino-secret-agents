$( document ).ready(function() {

    // Device
    var address = '192.168.1.105';
    var device = new Device(address);

    // Button
    $('#fw').click(function() {
      device.callFunction('forward', '');
    });

    $('#bw').click(function() {
      device.callFunction('backward', '');
    });

    $('#left').click(function() {
      device.callFunction('left', '');
    });

    $('#right').click(function() {
      device.callFunction('right', '');
    });

    $('#stop').click(function() {
      device.callFunction('stop', '');
    });

});

