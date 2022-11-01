package bragi.core.plugins.lavaplayer.manager.deezer;

import com.sedmelluq.discord.lavaplayer.tools.Units;
import com.sedmelluq.discord.lavaplayer.tools.io.*;
import com.sedmelluq.discord.lavaplayer.track.info.AudioTrackInfoProvider;
import java.io.BufferedInputStream;
import java.io.FileInputStream;
import java.io.FileOutputStream;
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
    private final FileOutputStream fileOutputStream;  //Поток записи в временный файл
    private final FileInputStream fileInputStream;  //Поток чтеия из временного файла
    private int currentChunk = 0;  //Номер текущего чанка (необходимо, потому что не все чанки нужно расшифровывать)
    private int position = 0;  //Текущая позиция трека

    private final Cipher cipher;  //Шифр, с помощью которого будем расшифровывать трек

    /* Основной конструктор */
    public DeezerAudioStream(String trackId, URL trackUrl) throws Exception {
        super(trackUrl.openConnection().getContentLength(), Units.CONTENT_LENGTH_UNKNOWN);

        //Название временного файла
        String filePath = "/tmp/" + trackId + ".flac";  //Устанавливаем название временного файла
        this.fileOutputStream = new FileOutputStream(filePath);  //Инициализируем поток для записи во временный файл
        this.fileInputStream = new FileInputStream(filePath);  //Инициализируем поток для чтения из веременного файла
        this.inputStream = new BufferedInputStream(trackUrl.openStream());  //Создаем поток для чтения зашифрованных данных

        /* Работаем с шифром Blowfish */
        SecretKeySpec key = new SecretKeySpec(GetKey(trackId).getBytes(), "Blowfish");  //Загружаем ключ для расшифровки данных
        IvParameterSpec iv = new IvParameterSpec(new byte[] {0, 1, 2, 3, 4, 5, 6, 7});  //Устанавливаем iv пареметр
        this.cipher = Cipher.getInstance("Blowfish/CBC/NoPadding");  //Выбираем шифр
        this.cipher.init(Cipher.DECRYPT_MODE, key, iv);  //Инициализируем шифр с использованием ключа и iv параметра
    }

    /* Метод для чтения одного байта */
    @Override
    public int read() throws IOException {
        int result = this.fileInputStream.read();  //Подгружаем данные из потока

        /* Если поток файла закончился, подгружаем туда новые данные */
        if (result == -1) {
            /* Если удаленный поток закончился, возвращаем -1, чтобы объявить о том, что поток закончился */
            if (UpdateChunk())
                result = this.fileInputStream.read();
            else
                result = clearTemp();
        }

        this.position++;  //Изменяем текущую позицию
        return result;
    }

    /* Метод для чтения нескольких байтов сразу */
    @Override
    public int read(byte @NotNull[] buffer, int offset, int length) throws IOException {
        int result = this.fileInputStream.read(buffer, offset, length);  //Подгружаем данные из потока

        /* Если поток файла закончился, подгружаем туда новые данные */
        if (result == -1) {
            /* Если удаленный поток закончился, возвращаем -1, чтобы объявить о том, что поток закончился (удаляя временный файл) */
            if (UpdateChunk())
                result = this.fileInputStream.read(buffer, offset, length);
            else
                result = clearTemp();
        }

        this.position += result;  //Изменяем текущую позицию
        return result;
    }

    /* Метод для подгрузки чанков */
    private boolean UpdateChunk() throws IOException {
        int chunkSize = 2048;  //Стандартный размер чанка
        int intervalChunk = 3;  //Интервал между чанками, которые расшифровывать не надо
        byte[] chunk = new byte[chunkSize];  //Объявляем пустой чанк
        int currentChunkSize = 0;  //Сюда будем запиисывать размер текущего чанка

        /* Подгружаем чанк вручную, потому что родной метод работает как-то нестабильно (чанки различаются по размеру) */
        for (int i = 0; i < chunkSize; i++)  {
            int value = this.inputStream.read();  //Получаем значение байта

            if (value != -1) {   //Если поток не закончился
                chunk[i] = (byte)value;  //Кладем его в чанк
                currentChunkSize++;
            }
            else  //Если поток закончился
                break;  //Выходим из цикла
        }

        /* Пробуем расшифровать чанк */
        try {
            /* Получаетися так, что не все чанки нужно расшифровывать */
            byte[] decryptedData;
            if (this.currentChunk % intervalChunk == 0)
                decryptedData = this.cipher.doFinal(chunk);
            else
                decryptedData = chunk;
            this.fileOutputStream.write(decryptedData, 0, chunkSize);  //Записываем данные в файл
        } catch (Exception ignore) {
            return false;  //При ошибке прекращаем проигрывание
        }

        this.currentChunk++;
        return currentChunkSize != 0;  //Если поток закончится, метод вернет значние false
    }

    /* Метод для удаления временных файлов */
    private int clearTemp() throws IOException {
        /* Закрываем все потоки */
        this.fileOutputStream.close();
        this.fileInputStream.close();
        this.inputStream.close();

        return -1;
    }

    /* Метод для генерации ключа */
    private String GetKey(String trackId) {
        /* Получаем ключ для расшифровки трека */
        String salt = "g4el58wc0zvf9na1";  //Соль, которая будет применяться вдальнейшем для расшифровки
        String hash = md5Hex(String.valueOf(trackId));  //Получаем MD5 хэш-сумму идентефикатора трека
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
    public long skip(long n) {
        return Units.CONTENT_LENGTH_UNKNOWN;
    }

    @Override
    public long getPosition() {
        return this.position;
    }

    @Override
    protected void seekHard(long position) {    }

    @Override
    public boolean canSeekHard() {
        return false;
    }

    @Override
    public List<AudioTrackInfoProvider> getTrackInfoProviders() {
        return Collections.emptyList();
    }
}