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

//play command
#define DIC_SLASH_PLAY "Воспроизвести трек"
#define DIC_SLASH_PLAY_QUERY "Запрос для поиска трека"
#define DIC_SLASH_PLAY_ATTACHMENT "Воспроизвести трек из вложений"
#define DIC_SLASH_PLAY_ATTACHMENT_ATTACHMENT "Трек для воспроизведения"

//errors
#define DIC_ERROR "**Ошибка**"
#define DIC_ERROR_ALREADY_IN_CURRENT_CHANNEL "**Бот уже подключен к этому голосовому каналу**"
#define DIC_ERROR_USER_NOT_IN_VOICE_CHANNEL "**Пользователь должен находиться в голосовом канале**"
#define DIC_ERROR_PERMISSION_DENIED "**Невозможно подключиться к голосовому каналу\nНедостаточно прав**"

#endif
