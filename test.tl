#测试用例

func help
	echo 欢迎使用Tursom Language (tlang) 系统
	echo 直接输入命令以调用命令
	echo 亦可以直接调用用户定义函数
	end

func add $1:Int $2:Int $ret:Int
	set ret i+ $1 $2
	echo $ret
	end

func sqrt $1:Double $ret:Double
	set ret sqrt $1
	echo $ret
	end

func square $1:Double $ret:Double
	set ret * $1 $1
	echo $ret
	end

echo package test.tl import completelyhelp
echo 使用"help"命令获取帮助