/*
 * Package : mqtt_client
 * Author : S. Hamblett <steve.hamblett@linux.com>
 * Date   : 10/07/2021
 * Copyright :  S.Hamblett
 *
 */

import 'dart:async';
import 'dart:io';
import 'package:mqtt_client/mqtt_server_client.dart';
import 'package:mqtt_client/mqtt_client.dart';

/// An example of connecting to the AWS IoT Core MQTT broker and publishing to a devices topic.
/// This example uses MQTT on port 8883 using certificites
/// More instructions can be found at https://docs.aws.amazon.com/iot/latest/developerguide/mqtt.html and
/// https://docs.aws.amazon.com/iot/latest/developerguide/protocols.html, please read this
/// before setting up and running this example.
Future<int> main() async {
  const url = 'iot.eclipse.org';  // Broker
  const port = 1883;
  const clientId = 'RedmiCassio';
  const topic = 'RainIOT_UNISC'; // Receba informações deste tópico


  // Create the client
  final client = MqttServerClient.withPort(url, clientId, port);

  // Connect the client
  try {
    print('MQTT client se conectando na url: $url, com id: $clientId');
    await client.connect();
  } on Exception catch (e) {
    print('MQTT client exception - $e');
    client.disconnect();
    exit(-1);
  }

  if (client.connectionStatus!.state == MqttConnectionState.connected) {
    print('MQTT client connected to AWS IoT');

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