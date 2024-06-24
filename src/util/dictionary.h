#ifndef BRAGI_DICTIONARY_H
#define BRAGI_DICTIONARY_H

//ping command
#define DIC_SLASH_PING "Получить задержку бота в мс"
#define DIC_PING "**Задержка**"
#define DIC_MS "**{} мс**"

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

//list command
#define DIC_SLASH_LIST_MSG_EMPTY_TITLE "**Список воспроизведения пуст**"
#define DIC_SLASH_LIST_MSG_TITLE "**Список воспроизведения ({}):**"
#define DIC_SLASH_LIST_FULL_TRACK_DATA "{} - {} ({})"

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

//http errors
#define DIC_HTTP_ERROR_CON_CANNOT_BE_ESTABLISHED "Соединение не может быть установлено"

#endif