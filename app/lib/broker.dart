
import 'dart:async';
import 'dart:io';
import 'package:mqtt_client/mqtt_server_client.dart';
import 'package:mqtt_client/mqtt_client.dart';


Future<int> main() async {
  // Your AWS IoT Core endpoint url
  const url = 'atx6yri73wi2d-ats.iot.us-east-2.amazonaws.com';
  // AWS IoT MQTT default port
  const port = 8883;
  // The client id unique to your device
  const clientId = 'bertold233420109asheley';

  // Create the client
  final client = MqttServerClient.withPort(url, clientId, port);

  // Set Keep-Alive
  client.keepAlivePeriod = 20;
  // Set the protocol to V3.1.1 for AWS IoT Core, if you fail to do this you will not receive a connect ack with the response code
  client.setProtocolV311();
  // logging if you wish
  client.logging(on: false);
// Set secure
  client.secure = true;

  final context = SecurityContext.defaultContext;
  context.setClientAuthorities('/home/cassio/Downloads/AmazonRootCA1.pem');
  context.useCertificateChain('/home/cassio/Downloads/.....crt');
  context.usePrivateKey('/home/cassio/Downloads/....pem.key');
  client.securityContext = context;

  // Setup the connection Message
  final connMess = MqttConnectMessage()
      .withClientIdentifier(clientId)
      .startClean();
  client.connectionMessage = connMess;

  // Connect the client
  try {
    print('MQTT client connecting to AWS IoT using certificates....');
    await client.connect();
  } on Exception catch (e) {
    print('MQTT client exception - $e');
    client.disconnect();
    exit(-1);
  }

  if (client.connectionStatus!.state == MqttConnectionState.connected) {
    print('MQTT client connected to AWS IoT');

    // Publish to a topic of your choice after a slight delay, AWS seems to need this
    await MqttUtilities.asyncSleep(1);
    const topic = '/test/topic';
    final builder = MqttClientPayloadBuilder();
    builder.addString('Hello World');
    // Important: AWS IoT Core can only handle QOS of 0 or 1. QOS 2 (exactlyOnce) will fail!
    client.publishMessage(topic, MqttQos.atLeastOnce, builder.payload!);

    // Subscribe to the same topic
    client.subscribe(topic, MqttQos.atLeastOnce);
    // Print incoming messages from another client on this topic
    client.updates!.listen((List<MqttReceivedMessage<MqttMessage>> c) {
      final recMess = c[0].payload as MqttPublishMessage;
      final pt =
      MqttPublishPayload.bytesToStringAsString(recMess.payload.message);
      print(
          'EXAMPLE::Change notification:: topic is <${c[0].topic}>, payload is <-- $pt -->');
      print('');
    });
  } else {
    print(
        'ERROR MQTT client connection failed - disconnecting, state is ${client.connectionStatus!.state}');
    client.disconnect();
  }

  print('Sleeping....');
  await MqttUtilities.asyncSleep(10);

  print('Disconnecting');
  client.disconnect();

  return 0;
}