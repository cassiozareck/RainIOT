#include <stdio.h>
#include <mosquitto.h>

#define MQTT_HOST "mqtt.eclipseprojects.io"
#define MQTT_PORT 1883
#define MQTT_TOPIC "RainIOT_UNISC"

void on_connect(struct mosquitto *mosq, void *obj, int rc) {
    if (rc == 0) {
        printf("Conectado ao broker MQTT.\n");
        mosquitto_subscribe(mosq, NULL, MQTT_TOPIC, 0);
    } else {
        fprintf(stderr, "Falha ao conectar ao broker MQTT.\n");
    }
}

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg) {
    printf("Mensagem recebida no tópico %s: %s\n", msg->topic, (char *)msg->payload);
}

int main(int argc, char *argv[]) {
    struct mosquitto *mosq;
    int rc;

    mosquitto_lib_init();
    mosq = mosquitto_new(NULL, true, NULL);
    if (!mosq) {
        fprintf(stderr, "Erro ao criar novo objeto Mosquitto.\n");
        return 1;
    }

    mosquitto_connect_callback_set(mosq, on_connect);
    mosquitto_message_callback_set(mosq, on_message);

    rc = mosquitto_connect(mosq, MQTT_HOST, MQTT_PORT, 60);
    if (rc != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "Não foi possível conectar ao broker MQTT: %s\n", mosquitto_strerror(rc));
        return 1;
    }

    rc = mosquitto_loop_forever(mosq, -1, 1);
    if (rc != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "Erro ao iniciar loop: %s\n", mosquitto_strerror(rc));
        return 1;
    }

    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    return 0;
}
