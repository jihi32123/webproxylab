#!/usr/bin/python

# nop-server.py - This is a server that we use to create head-of-line
#                 blocking for the concurrency test. It accepts a
#                 connection, and then spins forever.
#
# usage: nop-server.py <port>                
#


  #이 코드는 "nop-server.py"라는 Python 스크립트로, 동시성 테스트에서 head-of-line blocking을 생성하기 위해 사용하는 서버입니다. 
  # 이 서버는 연결을 수락하고, 그 후에 영원히 실행됩니다. 이 코드는 소켓을 생성하고, 지정된 포트 번호로 바인드한 다음, 서버를 대기 상태로 둡니다.
  # 클라이언트가 연결되면 무한 루프에서 대기하고, 아무런 작업도 수행하지 않습니다. 이렇게 함으로써, head-of-line blocking 문제를 시뮬레이션합니다.
import socket
import sys

#create an INET, STREAMing socket
serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
serversocket.bind(('', int(sys.argv[1])))
serversocket.listen(5)

while 1:
  channel, details = serversocket.accept()
  while 1:
    continue
  
  #이 코드는 "nop-server.py"라는 Python 스크립트로, 동시성 테스트에서 head-of-line blocking을 생성하기 위해 사용하는 서버입니다. 
  # 이 서버는 연결을 수락하고, 그 후에 영원히 실행됩니다. 이 코드는 소켓을 생성하고, 지정된 포트 번호로 바인드한 다음, 서버를 대기 상태로 둡니다.
  # 클라이언트가 연결되면 무한 루프에서 대기하고, 아무런 작업도 수행하지 않습니다. 이렇게 함으로써, head-of-line blocking 문제를 시뮬레이션합니다.