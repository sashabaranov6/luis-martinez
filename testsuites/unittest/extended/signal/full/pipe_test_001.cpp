/*
 * Copyright (c) 2013-2019 Huawei Technologies Co., Ltd. All rights reserved.
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "it_test_signal.h"
#include "signal.h"
#include "fcntl.h"

int TestPipeSingleProcess()
{
    int pipefd[2]; // 2, array subscript
    int retValue = -1;
    pid_t pid;
    retValue = pipe(pipefd);
    ICUNIT_ASSERT_EQUAL(retValue, 0, retValue);

    int *readFd = &pipefd[0];
    int *writeFd = &pipefd[1];

    char readbuffer[100]; // 100, array subscript
    int status, ret;

    int flag = fcntl(*readFd, F_GETFL);
    fcntl(*readFd, F_SETFL, flag | O_NONBLOCK);

    pid = fork();
    if (pid == -1) {
        printf("Fork Error!\n");
        return -1;
    } else if (pid == 0) {
        for (int i = 0; i < 3; i++) { // 3, Number of cycles
            errno = 0;
            char sentence1[15] = "Hello World";
            char a[4] = {0};
            (void)sprintf_s(a, sizeof(a), "%d", i);
            strcat(sentence1, a);
            int ret = write(*writeFd, sentence1, strlen(sentence1) + 1);
            usleep(100000); // 100000, Used to calculate the delay time.

            printf("read\n");
            memset(readbuffer, 0, sizeof(readbuffer));
            retValue = read(*readFd, readbuffer, sizeof(readbuffer));
            printf("Receive %d bytes data : %s,%d\n", retValue, readbuffer, errno);
            if (strncmp((readbuffer), (readbuffer), (strlen(sentence1))) != 0) {
                exit(errno);
            }
            usleep(100000); // 100000, Used to calculate the delay time.
        }
        exit(0);
    }
    ret = waitpid(pid, &status, 0);
    ICUNIT_ASSERT_EQUAL(ret, pid, ret);
    ICUNIT_ASSERT_EQUAL(WEXITSTATUS(status), 0, WEXITSTATUS(status));
    ret = unlink("/dev/pipe0");
    ICUNIT_ASSERT_NOT_EQUAL(ret, 0, ret);
    ret = close(pipefd[0]);
    ICUNIT_ASSERT_EQUAL(ret, 0, ret);
    ret = unlink("/dev/pipe0");
    ICUNIT_ASSERT_NOT_EQUAL(ret, 0, ret);
    ret = close(pipefd[1]);
    ICUNIT_ASSERT_EQUAL(ret, 0, ret);
    ret = unlink("/dev/pipe0");
    ICUNIT_ASSERT_EQUAL(ret, 0, ret);
    return 0;
}


void ItPosixPipe001(void)
{
    TEST_ADD_CASE(__FUNCTION__, TestPipeSingleProcess, TEST_POSIX, TEST_MEM, TEST_LEVEL0, TEST_FUNCTION);
}
