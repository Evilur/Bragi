#pragma once

#define _(TEXT) Locale::GetText(TEXT)

class Locale final {
public:
    static constexpr const char* GetText(const char* text) {
#define TR(TEXT1, TEXT2) if (strcmp(text, TEXT1) == 0) return TEXT2

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
        TR("{}ms",
           "{}мс");

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
#define DIC_JOINED "**:thumbsup: Бот присоединился к каналу `{}`**"

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
#define DIC_SLASH_LIST_MSG_EMPTY_TITLE "**Плейлист пуст**"
#define DIC_SLASH_LIST_MSG_TITLE "**Текщий плейлист ({}):**"
#define DIC_SLASH_LIST_FULL_TRACK_DATA "{}\u00A0\u202f\u2014\u00A0\u202f{}"

#define DIC_SLASH_NEXT "Выбрать следующий результат из поиска"
#define DIC_SLASH_NEXT_ORDINAL "Порядковый номер трека для замены"
#define DIC_SLASH_NEXT_NO_RESULTS "**По поисковому запросу больше не было найдено треков**"
#define DIC_SLASH_NEXT_PLAYLIST_EMPTY "**Плейлист пуст**"

//loop command
#define DIC_SLASH_LOOP "Включить/выключить повторение"
#define DIC_SLASH_LOOP_TYPE "Тип повторения"
#define DIC_SLASH_LOOP_TYPE_DISABLED "**:arrow_right: Повторение выключено**"
#define DIC_SLASH_LOOP_TYPE_TRACK "**:repeat_one: Включено повторение трека**"
#define DIC_SLASH_LOOP_TYPE_PLAYLIST "**:repeat: Включено повторение плейлиста**"

//play-attachment command
#define DIC_SLASH_PLAY_ATTACHMENT "Воспроизвести трек из вложений"
#define DIC_SLASH_PLAY_ATTACHMENT_ATTACHMENT "Трек для воспроизведения"

//track
#define DIC_TRACK_PLAYING_NOW "**:notes: Сейчас играет: `{}`"
#define DIC_TRACK_ADD_TO_PLAYLIST "**:notes: В плейлист добавлено: `{}`"
#define DIC_TRACK_DURATION ":watch: Продолжительность: `{}`**"
#define DIC_TRACK_ALBUM "**Альбом**"
#define DIC_TRACK_ARTIST "**Исполнитель**"
#define DIC_TRACK_LESS_THAN_MINUTE "{}с"
#define DIC_TRACK_LESS_THAN_HOUR "{}м{}с"
#define DIC_TRACK_MORE_THAN_HOUR "{}ч{}м{}с"

//errors
#define DIC_ERROR "**Ошибка**"
#define DIC_ERROR_BOT_IN_NOT_A_VOICE_CHANNEL "**Бот не находится в голосовом канале**"
#define DIC_ERROR_ALREADY_IN_CURRENT_CHANNEL "**Бот уже подключен к этому голосовому каналу**"
#define DIC_ERROR_USER_NOT_IN_VOICE_CHANNEL "**Пользователь должен находиться в голосовом канале**"
#define DIC_ERROR_PERMISSION_DENIED "**Невозможно подключиться к голосовому каналу\nНедостаточно прав**"
#define DIC_ERROR_NO_ATTACHMENTS "**Не было найдено ни одного вложения**"
#define DIC_ERROR_IS_NOT_A_FILE "**Файл из вложений не является аудио треком**"
#define DIC_ERROR_TRACK_NOT_FIND "**По данному запросу не было найдено ни одного результата**"
#define DIC_SLASH_NO_PARAMETER "**В команду не было передано обязательных параметров**"

//http errors
#define DIC_HTTP_ERROR_CON_CANNOT_BE_ESTABLISHED "Соединение не может быть установлено"