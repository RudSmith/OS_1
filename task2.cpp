// Как работает программа:
/*
1. Открываем оба файла, определяем размер, запускаем операцию чтения
2. На каждое завершение асинхронной операции вызывается aio_completion_handler
3. Как только завершается чтение, мы запускаем асинхронную запись, и, не дожидаясь её завершения, выходим из aio_completion_handler, возвращаемся в цикл в main и запускаем новое чтение
4. Возвращаемся в пункт 2

Таким образом, мы будем читать новый блок данных, не дожидаясь записи предыдущего блока, что и называется перекрывающимся вводом-выводом
*/

#define _GNU_SOURCE
#define MAX_OPERATIONS 16
#include <stdio.h>
#include <stdlib.h>
#include <aio.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <iostream>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <inttypes.h>
#include <time.h>
#include <chrono>

// структура для хранения информации об асинхронной операции
struct aio_operation 
{
    struct aiocb aio; // блок управления асинхронным вводом-выводом
    char *buffer; // буфер, куда читаем или откуда пишем
    int write_operation; // флаг (является ли операция операцией записи)
    off_t offset; // смещение (сколько байт уже прочитали)
    size_t block_size; // размер блока
};

// глобальные переменные для управления операциями
int fd_read, fd_write; // дескрипторы файлов
off_t file_size; // размер файла
size_t current_offset = 0; // сколько уже обработали байт
int operations_in_progress = 0; // кол-во выполняющихся в данный момент асинхронных операций
int read_complete = 0; // флаг завершения записи
// таймеры
std::chrono::_V2::system_clock::time_point start;
std::chrono::_V2::system_clock::time_point end;

// функция-обработчик завершения асинхронной операции
void aio_completion_handler(sigval_t sigval) 
{
    // получем указатель на информацию об операции, которая завершилась
    struct aio_operation *aio_op = (struct aio_operation *)sigval.sival_ptr;

    // проверяем наличие\отсутствие ошибки
    int err = aio_error(&aio_op->aio);
    if (err == 0) 
    {
        // aio return возвращает кол-во записанных\прочитанных байт либо 0, если достигнут конец файла (как обычные функции read и write)
        ssize_t bytes_processed = aio_return(&aio_op->aio);
        
        if (bytes_processed > 0) 
        {
            // если мы завершили операцию записи, выходим из функции, очистив память, и уже в main запускаем новую операцию чтения
            if (aio_op->write_operation) 
            {
                // операция записи завершена
                printf("Write operation completed at offset %ld, %zd bytes written.\n", aio_op->offset, bytes_processed);
            } 
            else 
            {
                // операция чтения завершена, нужно запускать асинхронную запись
                printf("Read operation completed at offset %ld, %zd bytes read.\n", aio_op->offset, bytes_processed);

                // запуск асинхронной записи, все поля заполняем аналогично операции чтения, за исключением write_operation и block_size
                struct aio_operation *write_op = (struct aio_operation*)malloc(sizeof(struct aio_operation));
                if (!write_op) 
                {
                    perror("Memory allocation failed for write_op");
                    free(aio_op->buffer);
                    free(aio_op);
                    return;
                }

                write_op->buffer = aio_op->buffer; // пишем из буфера последней выполненной операции чтения
                write_op->write_operation = 1; // выставляем флажок выполнения операции записи
                write_op->offset = aio_op->offset;
                write_op->block_size = bytes_processed; // размер блока не может превышать кол-во прочитанных байт в последней операции чтения

                memset(&write_op->aio, 0, sizeof(struct aiocb));
                write_op->aio.aio_fildes = fd_write; // дескриптор файла для записи
                write_op->aio.aio_buf = write_op->buffer;
                write_op->aio.aio_nbytes = bytes_processed; // опять же, ставим кол-во байт равное кол-ву прочитанных байт в последней операции чтения
                write_op->aio.aio_offset = write_op->offset;
                write_op->aio.aio_sigevent.sigev_notify = SIGEV_THREAD;
                write_op->aio.aio_sigevent.sigev_notify_function = aio_completion_handler;
                write_op->aio.aio_sigevent.sigev_value.sival_ptr = write_op;

                if (aio_write(&write_op->aio) < 0) 
                {
                    perror("aio_write failed");
                    free(write_op->buffer);
                    free(write_op);
                } 
                else 
                {
                    operations_in_progress++;
                }
            }
        }
    }
    else 
    {
        fprintf(stderr, "AIO operation failed: %s\n", strerror(err));
    }

    free(aio_op);
    operations_in_progress--;

    // если чтение завершено и все операции выполнены, завершаем программу
    if (read_complete && operations_in_progress == 0) 
    {
        printf("File copy completed successfully.\n");
        close(fd_read);
        close(fd_write);
        end = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        printf("time passed: %llu \n", elapsed.count());
        
        exit(0);
    }
}

// Функция для запуска асинхронного чтения
void start_read_operation(off_t offset, size_t block_size) 
{
    while (operations_in_progress >= MAX_OPERATIONS) {
        usleep(1000); // Ждем, пока количество операций уменьшится
    }
    // создаём структуру для асинхронной операции чтения
    struct aio_operation *read_op = (struct aio_operation*)malloc(sizeof(struct aio_operation));
    read_op->buffer = (char*)malloc(block_size); // выделяем память под данные для чтения
    if (!read_op->buffer) 
    {
        perror("Memory allocation failed for buffer");
        free(read_op);
        return;
    }

    read_op->write_operation = 0; // помечаем, что это НЕ операция записи
    read_op->offset = offset; // указываем в структуре, сколько байт мы уже прочитали
    read_op->block_size = block_size; // выставляем размер блока

    memset(&read_op->aio, 0, sizeof(struct aiocb));
    // заполняем блок управления асинхронной операцией
    read_op->aio.aio_fildes = fd_read; // указываем дескриптор файла
    read_op->aio.aio_buf = read_op->buffer; // указываем, куда будем читать
    read_op->aio.aio_nbytes = block_size; // выставляем размер блока 
    read_op->aio.aio_offset = offset; // выставляем, сколько уже прочитали
    read_op->aio.aio_sigevent.sigev_notify = SIGEV_THREAD; // указываем метод уведомления о завершении асинхронной операции, это у нас будет вызов sigev_notify_function
    read_op->aio.aio_sigevent.sigev_notify_function = aio_completion_handler; // указываем функцию sigev_notify_function
    read_op->aio.aio_sigevent.sigev_value.sival_ptr = read_op; // это у нас будет аргумент, передаваемый в функцию, мы передаём в неё read_op, чтобы в completion handler получить инфо о законченной операции

    // ставим в очередь запрос на чтение
    if (aio_read(&read_op->aio) < 0) 
    {
        perror("aio_read failed");
        free(read_op->buffer);
        free(read_op);
        return;
    }
    // увеличиваем счётчик операций
    operations_in_progress++;
}

int main(int argc, char *argv[]) 
{
    if (argc != 4) 
    {
        fprintf(stderr, "Usage: %s <source_file> <destination_file> <block_size>\n", argv[0]);
        return 1;
    }

    // читаем аргументы командной строки
    const char *read_filename = argv[1]; // откуда копируем
    const char *write_filename = argv[2]; // куда копируем
    size_t block_size = atoi(argv[3]); // размер блока

    // Открытие файлов (открываем их в неблокирующем режиме O_NONBLOCK)
    fd_read = open(read_filename, O_RDONLY | O_NONBLOCK, 0666);
    if (fd_read < 0) 
    {
        perror("Failed to open source file");
        return 1;
    }

    fd_write = open(write_filename, O_CREAT | O_WRONLY | O_TRUNC | O_NONBLOCK, 0666);
    if (fd_write < 0) 
    {
        perror("Failed to open destination file");
        close(fd_read);
        return 1;
    }

    start = std::chrono::system_clock::now();
    // Получение размера файла
    struct stat file_stat;
    fstat(fd_read, &file_stat);
    file_size = file_stat.st_size;

    // Запуск асинхронных операций чтения
    while (current_offset < file_size) 
    {
        if (operations_in_progress < MAX_OPERATIONS) 
        {
            // проверяем, сколько нам надо читать (если осталось читать меньше, чем размер блока, то читаем file_size - current_offset байт)
            size_t bytes_to_read = (current_offset + block_size > file_size) ? file_size - current_offset : block_size;
            if (bytes_to_read <= 0) break;
            // запускаем чтение
            start_read_operation(current_offset, bytes_to_read);
            // перемещаем указатель файла на столько байт, сколько прочитали
            current_offset += bytes_to_read;
        }
    }
    read_complete = 1;

    // Ожидание завершения всех операций
    while (operations_in_progress > 0) 
    {
        sleep(1);
    }

    return 0;
}