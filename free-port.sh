#!/bin/bash

#
# free-port.sh - returns an unused TCP port. 
#
# 이 코드는 Bash 스크립트로 작성된 free-port.sh 스크립트입니다. 이 스크립트는 사용 가능한 TCP 포트 번호를 찾아서 반환합니다.

# 코드의 주요 부분은 while 루프입니다. while 루프의 조건은 항상 참입니다([ TRUE ]), 따라서 루프가 끝나지 않습니다. 대신, while 루프 안에 있는 조건문을 사용하여 루프를 종료하거나 다음 포트 번호를 계산합니다.

# 루프가 시작되면 PORT_START 변수의 값(여기서는 4500)이 port 변수에 할당됩니다. 그런 다음, netstat 명령어를 사용하여 현재 시스템에서 사용 중인 모든 TCP 포트를 가져와서, 사용 중인 포트 목록에서 port 변수에 할당된 값과 일치하는 포트가 있는지 확인합니다.

# 만약 사용 중인 포트 목록에 port 변수에 할당된 값과 일치하는 포트가 있다면, port 변수에 1을 더해 다음 포트 번호를 계산합니다. 이 때, PORT_MAX 변수의 값(여기서는 65000)에 도달했는지 확인하고, 도달했다면 스크립트를 종료합니다.

# 그러나 사용 가능한 포트를 찾았다면, port 변수에 할당된 값을 출력하고 스크립트를 종료합니다.


PORT_START=4500
PORT_MAX=65000
port=${PORT_START}

while [ TRUE ] 
do
  portsinuse=`netstat --numeric-ports --numeric-hosts -a --protocol=tcpip | grep tcp | \
    cut -c21- | cut -d':' -f2 | cut -d' ' -f1 | grep -E "[0-9]+" | uniq | tr "\n" " "`

  echo "${portsinuse}" | grep -wq "${port}"
  if [ "$?" == "0" ]; then
    if [ $port -eq ${PORT_MAX} ]
    then
      exit -1
    fi
    port=`expr ${port} + 1`
  else
    echo $port
    exit 0
  fi
done
