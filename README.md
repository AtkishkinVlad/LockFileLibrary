# LockFileLibrary
Библиотека файловых блокировок на основе создания файлов &lt;myfile>.lck, где &lt;myfile> — имя блокируемого файла

## Установка

```bash
git clone https://github.com/AtkishkinVlad/LockFileLibrary.git
cd LockFileLibrary
cd Locker
make all
```

## Файловая структура

* removeLock.sh -> проверка реакции программы на удаление файла блокировки
* badPID_InLock1.sh -> проверка реакции программы на некорректный (нулевой) PID в файле блокировки
* badPID_InLock2.sh -> проверка программы на некорректный формат файла блокировки
* runme.sh -> запуск 10-ти программ, пытающихся получить доступ к файлу `work.txt`
* Makefile -> сборка программы locker
* main.c -> основной код программы

## Тестирование

```bash
В директории Locker вызвать make all (если не выполнили ранее)
bash runme.sh
```

## Результат тестирования

```bash
kotikvacia@Acer-Swift-3 /tmp/LockFileLibrary/Locker                                                                                                                       [15:15:15]
> $ make all                                                                                                                                                              [±main ●●]
gcc -c -W -Wall -O3  main.c
main.c: In function ‘handleError’:
main.c:48:5: warning: format not a string literal and no format arguments [-Wformat-security]
   48 |     fprintf(stderr, errorMessage);
      |     ^~~~~~~
main.c: In function ‘signalHandler’:
main.c:132:24: warning: unused parameter ‘sig’ [-Wunused-parameter]
  132 | void signalHandler(int sig) {
      |                    ~~~~^~~
gcc -o locker main.o -lm
                                                                                                                                                                                     
kotikvacia@Acer-Swift-3 /tmp/LockFileLibrary/Locker                                                                                                                       [15:15:20]
> $ bash runme.sh                                                                                                                                                         [±main ●●]
unlocking: lock file contains bad PID
unlocking: lock file contains bad PID
unlocking: lock file contains bad PID
unlocking: lock file does not exist
unlocking: lock file contains bad PID
unlocking: lock file contains bad PID
unlocking: lock file contains bad PID
unlocking: lock file contains bad PID
unlocking: lock file contains bad PID
unlocking: lock file contains bad PID
We have log:
Process with PID 2587 made 3 locks and 2 unlocks. Exited with erorr
Process with PID 2609 made 2 locks and 1 unlocks. Exited with erorr
Process with PID 2595 made 2 locks and 1 unlocks. Exited with erorr
Process with PID 2621 made 2 locks and 1 unlocks. Exited with erorr
Process with PID 2646 made 1 locks and 0 unlocks. Exited with erorr
Process with PID 2629 made 3 locks and 2 unlocks. Exited with erorr
Process with PID 2661 made 2 locks and 1 unlocks. Exited with erorr
Process with PID 2665 made 1 locks and 0 unlocks. Exited with erorr
Process with PID 2673 made 1 locks and 0 unlocks. Exited with erorr
Process with PID 2663 made 62 locks and 61 unlocks. Exited without error

kotikvacia@Acer-Swift-3 /tmp/LockFileLibrary/Locker                                                                                                                       [15:17:05]
> $ ls                                                                                                                                                                    [±main ●●]
Makefile  badPID_InLock1.sh  badPID_InLock2.sh  locker  log.txt  main.c  main.o  removeLock.sh  runme.sh  work.txt  work.txt.lck

                                                                                                                                                                                     
kotikvacia@Acer-Swift-3 /tmp/LockFileLibrary/Locker                                                                                                                       [15:20:40]
> $ cat work.txt.lock                                                                                                                                                     [±main ●●]
cat: work.txt.lock: No such file or directory
                                                                                                                                                                                     
kotikvacia@Acer-Swift-3 /tmp/LockFileLibrary/Locker                                                                                                                       [15:21:25]
> $ cat work.txt.lck                                                                                                                                                      [±main ●●]
2663

kotikvacia@Acer-Swift-3 /tmp/LockFileLibrary/Locker                                                                                                                       [15:21:33]
> $ cat log.txt                                                                                                                                                           [±main ●●]
Process with PID 2587 made 3 locks and 2 unlocks. Exited with erorr
Process with PID 2609 made 2 locks and 1 unlocks. Exited with erorr
Process with PID 2595 made 2 locks and 1 unlocks. Exited with erorr
Process with PID 2621 made 2 locks and 1 unlocks. Exited with erorr
Process with PID 2646 made 1 locks and 0 unlocks. Exited with erorr
Process with PID 2629 made 3 locks and 2 unlocks. Exited with erorr
Process with PID 2661 made 2 locks and 1 unlocks. Exited with erorr
Process with PID 2665 made 1 locks and 0 unlocks. Exited with erorr
Process with PID 2673 made 1 locks and 0 unlocks. Exited with erorr
Process with PID 2663 made 62 locks and 61 unlocks. Exited without error
```

**Выводы:**

Поскольку два системных вызова: 1) проверки отсутствия файла блокировки и 2) создания файла блокировки не составляют атомарную операцию, блокировка работает некорректно. 

Запускаются 10 конкурирующих процессов. 9 из них завершают свою работу по обнаружении ошибок процедур блокировки. Случайно оставшийся владельцем файла блокировки на момент смерти последнего конкурента процесс продолжает работать без сбоев, пока его не завершает
скрипт, отправляя ему сигнал SIGINT

