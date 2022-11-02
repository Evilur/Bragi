package bragi.core.plugins.lavaplayer.manager.deezer;

import com.sedmelluq.discord.lavaplayer.tools.Units;
import com.sedmelluq.discord.lavaplayer.tools.io.*;
import com.sedmelluq.discord.lavaplayer.track.info.AudioTrackInfoProvider;
import java.io.BufferedInputStream;
import java.io.IOException;
import java.net.URL;
import java.util.Collections;
import java.util.List;

import org.jetbrains.annotations.NotNull;

import javax.crypto.Cipher;
import javax.crypto.spec.IvParameterSpec;
import javax.crypto.spec.SecretKeySpec;

import static org.apache.commons.codec.digest.DigestUtils.md5Hex;

public final class DeezerAudioStream extends SeekableInputStream implements AutoCloseable {
    private final BufferedInputStream inputStream;  //Поток зашифрованных данных
    private final Cipher cipher;  //Шифр, с помощью которого будем расшифровывать трек
    private byte[] chunk = new byte[2048];  //Чанк, куда будет записываться расшифрованная информация
    private short localPosition = -1;  //Позиция байта в хранилище чанка
    private short currentChunk = 0;  //Номер текущего чанка (необходимо, потому что не все чанки нужно расшифровывать)
    private int totalPosition = 0;  //Текущая позиция трека

    /** Основной конструктор
     * @param trackId Id Deezer трека
     * @param trackUrl Url зашифрованного Deezer трека
     */
    public DeezerAudioStream(String trackId, URL trackUrl) throws Exception {
        super(trackUrl.openConnection().getContentLength(), Units.CONTENT_LENGTH_UNKNOWN);

        /* Создаем поток для чтения зашифрованных данных */
        this.inputStream = new BufferedInputStream(trackUrl.openStream());

        /* Работаем с шифром Blowfish */
        SecretKeySpec key = new SecretKeySpec(GetKey(trackId).getBytes(), "Blowfish");  //Ключ для расшифровки
        IvParameterSpec iv = new IvParameterSpec(new byte[] {0, 1, 2, 3, 4, 5, 6, 7});  //Устанавливаем iv параметр
        this.cipher = Cipher.getInstance("Blowfish/CBC/NoPadding");  //Выбираем шифр
        this.cipher.init(Cipher.DECRYPT_MODE, key, iv);  //Инициализируем шифр с использованием ключа и iv параметра
    }

    /**
     * Метод для чтения одного байта
     * @return Либо байт, если он есть, либо -1, если поток уже пуст
     */
    @Override
    public int read() throws IOException {
        if (UpdateChunk()) {  //Если поток не закончился
            /* Обновляем позицию */
            this.localPosition = (short)(this.localPosition < 2048 ? this.localPosition + 1 : -1);

            if (this.localPosition != -1) {  //Если чанк еще не закончился
                this.totalPosition++;  //Обновляем позицию
                int result = this.chunk[this.localPosition - 1];  //Здесь хранится результат

                /* Возвращаем результат */
                if (result == -1) return 255;
                else return result;
            } else return this.read();  //Если чанк закончился
        } else return -1;  //Если поток закончился
    }

    /** Метод для чтения нескольких байтов сразу
     * @param buffer Буффер, в который будет записана информация
     * @param offset Смещение, с которым будет считываться информация
     * @param length Максимальное количество байтов для считывания
     * @return Либо количество байт, если они есть, либо -1, если поток уже пуст
     */
    @Override
    public int read(byte @NotNull[] buffer, int offset, int length) throws IOException {
        if (UpdateChunk()) {  //Если поток не закончился.
            int result = 0;  //Переменная для вывода
            for (int i1 = 0; i1 < length && this.localPosition < 2048; i1++, this.localPosition++) {
                buffer[i1] = this.chunk[this.localPosition];
                result++;
            }

            /* Обновляем позицию */
            this.localPosition = this.localPosition < 2048 ? this.localPosition : -1;
            this.totalPosition += length;

            return result;  //Возвращаем результат
        } else  //Если поток закончился
            return -1;
    }

    /** Метод для подгрузки чанков
     * @return true, если поток не окончен, false - в ином случае
     * @throws IOException Выбрасывает исключение, если не удалось расшифровать чанк
     * 2048 - константный размер чанка
     */
    private boolean UpdateChunk() throws IOException {
        /* Проверяем, надо ли подгружать новый чанк, или еще рано */
        if (this.localPosition != -1) return true;
        int intervalChunk = 3;  //Интервал между чанками, которые расшифровывать не надо
        int currentChunkSize = 0;  //Сюда будем записывать размер текущего чанка

        /* Подгружаем чанк вручную, т.к. родной метод работает нестабильно */
        for (int i = 0; i < 2048; i++)  {
            int value = this.inputStream.read();  //Получаем значение байта

            if (value != -1) {   //Если поток не закончился
                this.chunk[i] = (byte)value;  //Кладем его в чанк
                currentChunkSize++;
            } else break;  //Если поток закончился, выходим из цикла
        }

        /* Пробуем расшифровать чанк */
        try {
            /* Получается так, что не все чанки нужно расшифровывать */
            if (this.currentChunk % intervalChunk == 0) //Если надо, расшифровываем
                this.chunk = this.cipher.doFinal(this.chunk);
        } catch (Exception ignore) {
            System.out.println("Не удалось расшифровать чанк");
            return false;  //При ошибке прекращаем проигрывание
        }

        this.currentChunk++;
        this.localPosition++;
        return currentChunkSize != 0;  //Если поток закончился, возвращаем false, иначе - true
    }

    /** Метод для генерации ключа
     * @param trackId Id трека Deezer
     * @return Ключ для расшифровки
     */
    private String GetKey(String trackId) {
        /* Получаем ключ для расшифровки трека */
        String salt = "g4el58wc0zvf9na1";  //Соль, которая будет применяться в дальнейшем для расшифровки
        String hash = md5Hex(String.valueOf(trackId));  //Получаем MD5 хэш-сумму идентификатора трека
        String firstMD5Half = hash.substring(0, 16);  //Получаем первую половину хэш-суммы
        String secondMD5Half = hash.substring(16, 32);  //Получаем вторую половину хэш-суммы
        StringBuilder keyBuilder = new StringBuilder();  //В этой переменной будем хранить значение ключа

        /* Собираем ключ */
        for (byte i = 0; i < 16; i++) {
            int charCode = salt.charAt(i) ^ (int)firstMD5Half.charAt(i) ^ (int)secondMD5Half.charAt(i);
            keyBuilder.append((char) charCode);
        }  /* На этом этапе мы получили ключ для расшифровки трека */

        return keyBuilder.toString();  //Возвращаем ключ
    }

    @Override
    public long skip(long n) { return Units.CONTENT_LENGTH_UNKNOWN; }
    @Override
    public long getPosition() { return this.totalPosition; }
    @Override
    protected void seekHard(long position) {    }
    @Override
    public boolean canSeekHard() { return false; }
    @Override
    public List<AudioTrackInfoProvider> getTrackInfoProviders() {
        return Collections.emptyList();
    }
}