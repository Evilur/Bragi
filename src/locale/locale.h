#pragma once

#include <cstring>

#define _(TEXT) Locale::GetText(TEXT)

class Locale final {
public:
    static consteval const char* GetText(const char* text) {
#define TR(TEXT1, TEXT2) if (strcmp(text, TEXT1) == 0) return TEXT2

        /* Join command */
        TR("**The user must be in the voice channel**",
           "**Пользователь должен находиться в голосовом канале**");
        TR("**The bot is already connected to this voice channel**",
           "**Бот уже подключен к этому голосовому каналу**");
        TR("**Unable to connect to the voice channel.\n"
           "Insufficient rights**",
           "**Невозможно подключиться к голосовому каналу\n"
           "Недостаточно прав**");
        TR("**:thumbsup: The bot has joined the channel `%s`**",
           "**:thumbsup: Бот присоединился к каналу `%s`**");

        /* Play command */
        TR("**No results have been found for this query**",
           "**По данному запросу не было найдено ни одного результата**");

        /* Leave command */
        TR("**:person_walking: Bot has left the voice channel**",
           "**:person_walking: Бот покинул голосовой канал**");

        /* List command */
        TR("**Current playlist:**",
           "**Текущий плейлист:**");
        TR("**Playlist is empty**",
           "**Плейлист пуст**");

        /* Loop command */
        TR("**:repeat_one: Track repeat enabled**",
           "**:repeat_one: Повторение трека включено**");
        TR("**:repeat: Playlist repeat enabled**",
           "**:repeat: Повторение плейлиста включено**");
        TR("**:arrow_right: Repeat disabled**",
           "**:arrow_right: Повторение отключено**");

        /* Speed command */
        TR("**:asterisk: Playback speed: `{}%`**",
           "**:asterisk: Скорость воспроизведения: `{}%`**");
        TR("**Minimum speed - 25%**",
           "**Минимальная скорость - 25%**");
        TR("**Maximum speed - 250%**",
           "**Максимальная скорость - 250%**");

        /* Ping command */
        TR("Ping",
           "Задержка");
        TR("%.0fms",
           "%.0fмс");

        /* Bragi exception */
        TR("**Error**",
           "**Ошибка**");

        /* If there is no translation for this text, return the source */
        return text;
    }
};

//ping command
#define DIC_SLASH_PING "Получить задержку бота в мс"

//join command
#define DIC_SLASH_JOIN "Присоединиться к голосовому каналу"
#define DIC_SLASH_JOIN_USER "Пользователь, к которому необходимо присоединиться"

//leave command
#define DIC_SLASH_LEAVE "Покинуть голосовой канал"
#define DIC_LEFT "**:person_walking: Бот покинул голосовой канал**"

//play command
#define DIC_SLASH_PLAY "Воспроизвести трек"
#define DIC_SLASH_PLAY_QUERY "Запрос для поиска трека"

//skip command
#define DIC_SLASH_SKIP "Пропустить текущий трек"
#define DIC_SLASH_SKIP_NUMBER "Количество треков для пропуска"
#define DIC_SKIP_PLAYLIST_IS_EMPTY "**В плейлисте нет треков**"
#define DIC_SKIP_MSG "**:track_next: Треков пропущено: `{}`**"
#define DIC_SKIP_WRONG_NUM_FOR_SKIP "**Невозможно пропустить такое количество треков**"

//speed command
#define DIC_SLASH_SPEED "Изменить скорость воспроизведения трека"
#define DIC_SLASH_SPEED_PERCENT "Процент скорости воспроизведения"

//list command
#define DIC_SLASH_LIST "Вывести текущий список воспроизведения"
#define DIC_SLASH_LIST_FULL_TRACK_DATA "{}\u00A0\u202f\u2014\u00A0\u202f{}"

#define DIC_SLASH_NEXT "Выбрать следующий результат из поиска"
#define DIC_SLASH_NEXT_ORDINAL "Порядковый номер трека для замены"
#define DIC_SLASH_NEXT_NO_RESULTS "**По поисковому запросу больше не было найдено треков**"
#define DIC_SLASH_NEXT_PLAYLIST_EMPTY "**Плейлист пуст**"

//loop command
#define DIC_SLASH_LOOP "Включить/выключить повторение"
#define DIC_SLASH_LOOP_TYPE "Тип повторения"

//track
#define DIC_TRACK_PLAYING_NOW "**:notes: Сейчас играет: `{}`"
#define DIC_TRACK_ADD_TO_PLAYLIST "**:notes: В плейлист добавлено: `{}`"
#define DIC_TRACK_DURATION ":watch: Продолжительность: `{}`**"
#define DIC_TRACK_ALBUM "**Альбом**"
#define DIC_TRACK_ARTIST "**Исполнитель**"

//errors
#define DIC_ERROR_BOT_IN_NOT_A_VOICE_CHANNEL "**Бот не находится в голосовом канале**"