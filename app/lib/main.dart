import 'dart:io';
import 'dart:math';

import 'package:flutter/material.dart';
import 'package:mqtt_client/mqtt_client.dart';
import 'package:mqtt_client/mqtt_server_client.dart';
import 'package:flutter_local_notifications/flutter_local_notifications.dart';

void main() => runApp(const MyApp());

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'RainIOT',
      theme: ThemeData(primarySwatch: Colors.blue),

      // Pagína HOME será MQTTScreen
      home: const MQTTScreen(),
    );
  }
}

class MQTTScreen extends StatefulWidget {
  const MQTTScreen({super.key});

  @override
  _MQTTScreenState createState() => _MQTTScreenState();
}

class _MQTTScreenState extends State<MQTTScreen> {
  final FlutterLocalNotificationsPlugin flutterLocalNotificationsPlugin =
  FlutterLocalNotificationsPlugin();
  MqttServerClient? client;

  @override
  void initState() {
    super.initState();
    connectMQTT();
    initializeNotifications();
  }
  // Inicializa as notificações com as configurações escolhidas
  Future<void> initializeNotifications() async {
    const AndroidInitializationSettings initializationSettingsAndroid =
    AndroidInitializationSettings('@mipmap/ic_launcher'); // Ícone usado no APP
    const InitializationSettings initializationSettings =
    InitializationSettings(
      android: initializationSettingsAndroid,
    );
    await flutterLocalNotificationsPlugin.initialize(initializationSettings);
  }

  // Conecta no broker MQTT de forma assíncrona para ouvir novas mensagens sem
  // trancar a thread principal
  Future<void> connectMQTT() async {
    const url = '44cd3984802a4140a875c758be49bbac.s2.eu.hivemq.cloud';
    const port = 8883;

    // Gera um número único e aleatório para o Client ID
    final random = Random();
    final clientId = 'randomuser${random.nextInt(500000)}';

    client = MqttServerClient.withPort(url, clientId, port);
    client!.keepAlivePeriod = 20;
    client!.secure = true;
    client!.securityContext = SecurityContext.defaultContext;

    await client!.connect('cassiopc123123', "C123c321");
    subscribeToTopic();
  }

  // Se inscreve no tópico rainiot que o ESP32 vai enviar mensagem
  void subscribeToTopic() {
    const topic = 'rainiot';
    client!.subscribe(topic, MqttQos.exactlyOnce);
    client!.updates!.listen((List<MqttReceivedMessage<MqttMessage>> c) {
      print("Nova mensagem");
      // Quando receber mensagem:
      final recMess = c[0].payload as MqttPublishMessage;
      final pt =
      MqttPublishPayload.bytesToStringAsString(recMess.payload.message);
      if (pt == '1') {
        showNotification('Está chovendo, olha a roupa no varal', '');
      } else {
        showNotification('Não está chovendo', '');
      }
    });
  }

  Future<void> showNotification(String title, String body) async {
    const AndroidNotificationDetails androidPlatformChannelSpecifics =
    AndroidNotificationDetails(
      'channel_id',
      'Channel',
      importance: Importance.max,
      priority: Priority.high,
    );
    const NotificationDetails platformChannelSpecifics =
    NotificationDetails(android: androidPlatformChannelSpecifics);

    await flutterLocalNotificationsPlugin.show(
      0, // Notification ID
      title,
      body,
      platformChannelSpecifics,
      payload: 'notification_payload',
    );
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('RainIOT')),
      body: const Center(child: Text('Estamos ouvindo seu dispositivo')),
    );
  }

  @override
  void dispose() {
    client?.disconnect();
    super.dispose();
  }
}
