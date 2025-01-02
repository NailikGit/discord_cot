#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <concord/discord.h>
#include <concord/log.h>

u64snowflake g_app_id;

void print_usage() {
  printf("\n\nThis bot demonstrates a simple ping-pong response.\n"
         "1. Type 'pong' in chat\n"
         "2. Tpye 'ping' in chat\n"
         "\nTYPE ANY KEY TO START BOT\n");
}

void on_ready(struct discord* client, const struct discord_ready* event) {
  log_info("ping_pong_bot succesfully connected to Discord as %s#%s!",
           event->user->username, event->user->discriminator);

  g_app_id = event->application->id;
}

void on_ping(struct discord* client, const struct discord_message* event) {
  if(event->author->bot) return;

  struct discord_create_message params = {.content = "pong"};
  discord_create_message(client, event->channel_id, &params, NULL);
}

void on_pong(struct discord* client, const struct discord_message* event) {
  if(event->author->bot) return;

  struct discord_create_message params = {.content = "ping"};
  discord_create_message(client, event->channel_id, &params, NULL);
}

void on_slash_command_create(struct discord* client, const struct discord_message* event) {
  struct discord_application_command_option sub_user[] = {
    { .type = DISCORD_APPLICATION_OPTION_USER,
      .name = "user",
      .description = "user to make to a nutte" },
    { .type = DISCORD_APPLICATION_OPTION_STRING,
      .name = "reason",
      .description = "reason why person/role is a nutte"
    }
  };
  struct discord_application_command_option sub_role[] = {
    { .type = DISCORD_APPLICATION_OPTION_ROLE,
      .name = "role",
      .description = "role to make to a nutte" },
    { .type = DISCORD_APPLICATION_OPTION_STRING,
      .name = "reason",
      .description = "reason why person/role is a nutte"
    }
  };

  struct discord_application_command_option options[] = {
    { .type = DISCORD_APPLICATION_OPTION_SUB_COMMAND,
      .name = "role",
      .description = "role to make to a nutte",
      .options = &(struct discord_application_command_options){.size = 2, .array = sub_role}},
    { .type = DISCORD_APPLICATION_OPTION_SUB_COMMAND,
      .name = "user",
      .description = "user to make to a nutte",
      .options = &(struct discord_application_command_options){.size = 2, .array = sub_user}
    }
  };

  struct discord_create_guild_application_command params[] = {
    { .name = "ping", .description = "simple ping-pong command" },
    { .name = "nutte",
      .description = "makes someone a nutte",
      .options = &(struct discord_application_command_options) {
        .size = 2,
        .array = options
      }
    }
  };

  for(int i = 0; i < 2; i++) {
    discord_create_guild_application_command(client, g_app_id, event->guild_id, &params[i], NULL);
  }
}

void on_interaction(struct discord* client, const struct discord_interaction* event) {
  if(event->type != DISCORD_INTERACTION_APPLICATION_COMMAND) return;
  if(!event->data) return;

  struct discord_interaction_response params;
  if(strcmp(event->data->name, "ping") == 0) {
    params.type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE;
    params.data = &(struct discord_interaction_callback_data) {.content = "pong"};
  } else if(strcmp(event->data->name, "nutte") == 0) {
    if(!event->data->options) return;
    params.type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE;
    char buf[DISCORD_MAX_MESSAGE_LEN];

    /* printf("\n"); */
    /* printf("%s\n", event->data->name); */
    /* printf("%s, %s\n", event->data->options->array[0].name, event->data->options->array[0].options->array[0].value); */
    /* printf("%s\n", event->data->options->array[0].options->array[1].value); */
    /* printf("\n\n"); */

    if(strcmp(event->data->options->array[0].name, "user") == 0) {
      if(event->data->options->array[0].options->realsize == 1)
        snprintf(buf, DISCORD_MAX_MESSAGE_LEN, "<@%s> ist eine nutte", event->data->options->array[0].options->array[0].value);
      else
        snprintf(buf, DISCORD_MAX_MESSAGE_LEN, "<@%s> ist eine nutte, %s",
                                               event->data->options->array[0].options->array[0].value,
                                               event->data->options->array[0].options->array[1].value);
    } else {
      if(event->data->options->array[0].options->realsize == 1)
        snprintf(buf, DISCORD_MAX_MESSAGE_LEN, "<@&%s> sind nutten", event->data->options->array[0].options->array[0].value);
      else
        snprintf(buf, DISCORD_MAX_MESSAGE_LEN, "<@&%s> sind nutten, %s",
                                               event->data->options->array[0].options->array[0].value,
                                               event->data->options->array[0].options->array[1].value);
    }

    params.data = &(struct discord_interaction_callback_data) {.content = buf};
  }
  discord_create_interaction_response(client, event->id, event->token, &params, NULL);
}

int main(int argc, char * argv[]) {
  const char* config_file;
  if(argc > 1) config_file = argv[1];
  else config_file = "../config.json";

  char command1[512];
  sprintf(command1, "../replace.sh %s", config_file);
  system(command1);

  ccord_global_init();
  struct discord* client = discord_config_init(config_file);

  char command2[512];
  sprintf(command2, "../remove.sh %s", config_file);
  system(command2);

  discord_set_on_command(client, "slash_create", &on_slash_command_create);
  discord_set_on_ready(client, &on_ready);
  discord_set_on_interaction_create(client, &on_interaction);
  discord_set_on_command(client, "ping", &on_ping);
  discord_set_on_command(client, "pong", &on_pong);

  /* print_usage(); */
  /* fgetc(stdin); */

  discord_run(client);

  discord_cleanup(client);
  ccord_global_cleanup();

  return 0;
}
