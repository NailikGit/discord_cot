#include <concord/discord_codecs.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <concord/discord.h>
#include <concord/log.h>

u64snowflake g_app_id;

void on_ready(struct discord* client, const struct discord_ready* event) {
  log_info("nutte succesfully connected to Discord as %s#%s!",
           event->user->username, event->user->discriminator);

  g_app_id = event->application->id;
}

void on_ping(struct discord* client, const struct discord_message* event) {
  if(event->author->bot) return;

  struct discord_create_message params = {.content = "pong"};
  log_info("function on_ping sending message '%s'", params.content);
  discord_create_message(client, event->channel_id, &params, NULL);
}

void on_pong(struct discord* client, const struct discord_message* event) {
  if(event->author->bot) return;

  struct discord_create_message params = {.content = "ping"};
  log_info("function on_pong sending message '%s'", params.content);
  discord_create_message(client, event->channel_id, &params, NULL);
}

void on_they_slash_them(struct discord* client, const struct discord_message* event) {
  if(event->author->bot) return;

  // event->message_reference->fail_if_not_exists = false;

  struct discord_create_message params = {.content = "WOKE",
                                          .message_reference = &(struct discord_message_reference){
                                            .guild_id = event->guild_id,
                                            .channel_id = event->channel_id,
                                            .message_id = event->id,
                                            .fail_if_not_exists = false
                                          }};
  log_info("function on_pong sending message '%s' as reply to user %lu",
           params.content, event->author->id);
  discord_create_message(client, event->channel_id, &params, NULL);
}

void on_slash_command_create(struct discord* client, const struct discord_message* event) {
  struct discord_application_command_option sub_user[] = {
    { .type = DISCORD_APPLICATION_OPTION_USER,
      .name = "user",
      .description = "user to make to a nutte",
      .required = true},
    { .type = DISCORD_APPLICATION_OPTION_STRING,
      .name = "reason",
      .description = "reason why person/role is a nutte"
    }
  };
  struct discord_application_command_option sub_role[] = {
    { .type = DISCORD_APPLICATION_OPTION_ROLE,
      .name = "role",
      .description = "role to make to a nutte",
      .required = true},
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
    { .name = "pong", .description = "simple ping-pong command" },
    { .name = "nutte",
      .description = "makes someone a nutte",
      .options = &(struct discord_application_command_options) {
        .size = 2,
        .array = options
      }
    }
  };

  for(int i = 0; i < 2; i++) {
    log_info("creating slash command '%s'", params[i].name);
    discord_create_guild_application_command(client, g_app_id, event->guild_id, &params[i], NULL);
  }
}

void on_interaction(struct discord* client, const struct discord_interaction* event) {
  if(!event->data || event->type != DISCORD_INTERACTION_APPLICATION_COMMAND) {
    log_info("invalid slash command");
    return;
  }

  struct discord_interaction_response params;
  if(strcmp(event->data->name, "ping") == 0) {
    params.type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE;
    params.data = &(struct discord_interaction_callback_data) {.content = "pong"};
  } else if(strcmp(event->data->name, "pong") == 0) {
    params.type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE;
    params.data = &(struct discord_interaction_callback_data) {.content = "ping"};
  } else if(strcmp(event->data->name, "nutte") == 0) {
    if(!event->data->options) {
      log_info("invalid slash command");
      return;
    }
    params.type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE;
    char buf[DISCORD_MAX_MESSAGE_LEN];

    if(strcmp(event->data->options->array[0].name, "user") == 0) {
      if(event->data->options->array[0].options->realsize == 1)
        snprintf(buf, DISCORD_MAX_MESSAGE_LEN, "<@%s> ist eine nutte",
                 event->data->options->array[0].options->array[0].value);
      else
        snprintf(buf, DISCORD_MAX_MESSAGE_LEN, "<@%s> ist eine nutte, %s",
                 event->data->options->array[0].options->array[0].value,
                 event->data->options->array[0].options->array[1].value);
    } else {
      if(event->data->options->array[0].options->realsize == 1)
        snprintf(buf, DISCORD_MAX_MESSAGE_LEN, "<@&%s> sind nutten",
                 event->data->options->array[0].options->array[0].value);
      else
        snprintf(buf, DISCORD_MAX_MESSAGE_LEN, "<@&%s> sind nutten, %s",
                 event->data->options->array[0].options->array[0].value,
                 event->data->options->array[0].options->array[1].value);
    }

    params.data = &(struct discord_interaction_callback_data) {.content = buf};
  }

  log_info("sending '%s' as response to slash command by '%lu'",
           params.data->content, event->member->user->id);
  discord_create_interaction_response(client, event->id, event->token, &params, NULL);
}

int main(int argc, char * argv[]) {
  const char* config_file;
  if(argc > 1) config_file = argv[1];
  else config_file = "config.json";

  system("pwd");
  char command1[512];
  sprintf(command1, "scripts/replace.sh %s", config_file);
  system(command1);

  ccord_global_init();
  struct discord* client = discord_config_init(config_file);

  char command2[512];
  sprintf(command2, "scripts/remove.sh %s", config_file);
  system(command2);

  discord_set_on_command(client, "slash_create", &on_slash_command_create);
  discord_set_on_ready(client, &on_ready);
  discord_set_on_interaction_create(client, &on_interaction);
  discord_set_on_command(client, "ping", &on_ping);
  discord_set_on_command(client, "pong", &on_pong);
  discord_set_on_command(client, "they_slash_them", &on_they_slash_them);

  discord_run(client);

  discord_cleanup(client);
  ccord_global_cleanup();

  log_info("nutte closed successfully");

  return 0;
}
