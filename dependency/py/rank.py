import os
import json
import re

winGamesMap = {}
totalGamesMap = {}
studentTotalGame = {}
trickerTotalGame = {}
winRateMap = {}
studentWinMap = {}
trickerWinMap = {}
studentWinRateMap = {}
trickerWinRateMap = {}

winGamesMap1 = {}
totalGamesMap1 = {}
studentTotalGame1 = {}
trickerTotalGame1 = {}
winRateMap1 = {}
studentWinMap1 = {}
trickerWinMap1 = {}
studentWinRateMap1 = {}
trickerWinRateMap1 = {}

studentWinNum = 0
trickerWinNum = 0
totalGameNum = 0

studentWinNum1 = 0
trickerWinNum1 = 0
totalGameNum1 = 0

teamIDtoName = {
    "951c89eb-aa9c-45d4-af10-ad9785a047d6": "无名万物之始", "2e504ec6-50b1-4985-b2fd-995927ea9717": "LQ说什么都队", "fb472ad6-65e0-494b-a7be-f6f25ecda018": "是啊我诶",
    "3376909b-5ddb-41ab-994c-3c5b5ba60158": "叛逃者联盟",
    "9b0f2257-734c-42e2-8061-c267e3d0d231": "ChatGPA",
    "2de908c9-1b99-4811-ae00-68140d1c4527": "昊天上帝和他的三个神父",
    "94866510-af51-439c-a41a-b4cb42596e25": "少年毕不了业",
    "4613ef48-4988-4508-a258-66857a3250b8": "PKT48TeamTS",
    "8e14b2a3-fc37-4fb6-b8ac-a722a10707d7": "京ICP备2022019669号-1 2022 EESAST",
    "e8db213c-a636-483f-a6ed-84310b3093a4": "拉拉队",
    "04abd472-ed7a-4840-8680-87d20db52721": "努力少女戏尔危",
    "2bc1b761-ace3-4403-af83-e46ca328bcd0": "测试",
    "4c1d6333-e25c-4b0f-bc06-9851db446bd7": "摆烂吧,少女",
    "7f819704-99c0-41d8-bc61-26eec0bd73bb": "一会吃萤火虫",
    "de915bbf-0751-4a9d-ab30-a470807406b2": "小小做题家",
    "28baa2bf-5130-4a1e-ab9c-36e8faf87f84": "数析少女队",
    "5c868c42-3b07-4280-a825-a6f80e0d5a2c": "沙壁北京",
    "ed03d1ac-810a-4547-b54a-d56cd5213364": "我会出手队",
    "5d59e45f-cb0e-4294-90f4-282adb1d476d": "闪电骑士团",
    "637e20c1-a904-4f6f-b706-2cea7c4daf5e": "Mukava Poikaa",
    "7185eb49-0cb0-43c0-a469-a39e636d66d4": "劝退吧,少女",
    "d2a7ba71-4a86-4278-a362-8a8953a368f8": "纵火犯在何方",
    "4e266301-7749-4699-b2de-511d458cf537": "土木清华没有水",
    "454f37bd-2f54-4463-94d9-2df9aedb4f21": "电电做不队",
    "a2573713-28e4-4af7-8c53-ab21f385f789": "王牌飞行队",
    "194c3ddf-6846-47ec-a262-ca24f5639531": "快乐Debug",
    "97cf5969-e8ff-410e-b85c-0c8359923289": "卷动量守恒",
    "4646739a-9ff5-4854-a3b2-27d5b85ea504": "龙井队",
    "c431d105-a2b3-4659-b713-6bc97132ec7f": "疯狂Thurs队",
    "9ee48de1-a76a-40eb-b267-59c985bbe6cb": "蒸馍",
    "5ef8ffbb-0776-4a74-a84f-3d80d5b4c2ae": "你说什么都队",
    "65f94306-69c7-42a2-8c68-44cb45749aae": "closeAI",
    "ab0406ae-6a0e-4c1e-9d36-eb14115de076": "N/A",
    "82cbff06-9ed1-429b-afc3-7e050318bf93": "代码一行都不队",
    "6b52346c-4528-424b-ac75-22fa573ebaad": "pqfobj",
    "93e7f3f1-d47f-4588-b433-72877089f0bd": "你说得队",
    "2f6f9ce3-f2d3-4799-b291-38dc04d048a0": "少女终末旅行",
    "07c0ad6c-f612-4375-9b79-52bb89c79d76": "大括号换行委员会",
    "bdf5b1c5-4dbc-4589-a6bc-8c5932c04de7": "孤客若风",
    "f0d75eee-34a6-4484-8e23-720d26db747d": "/",
    "acea715f-d5b0-4113-b6c3-9f6d7822f2e9": "难崩"
}

dirs = os.listdir(".")
for dir in dirs:
    if dir.startswith("Team"):
        dirdir = dir.replace("Team_", "")
        dirdir = dirdir.split("--vs--")
        try:
            with open(f"{dir}/result.json", 'r') as f:
                result = json.load(f)
                for i in (0, 1):
                    if dirdir[i] not in winGamesMap:
                        winGamesMap[dirdir[i]] = 0
                    if dirdir[i] not in totalGamesMap:
                        totalGamesMap[dirdir[i]] = 0
                    if dirdir[i] not in studentWinMap:
                        studentWinMap[dirdir[i]] = 0
                    if dirdir[i] not in trickerWinMap:
                        trickerWinMap[dirdir[i]] = 0
                    if dirdir[i] not in studentTotalGame:
                        studentTotalGame[dirdir[i]] = 0
                    if dirdir[i] not in trickerTotalGame:
                        trickerTotalGame[dirdir[i]] = 0
                    if dirdir[i] not in winGamesMap1:
                        winGamesMap1[dirdir[i]] = 0
                    if dirdir[i] not in totalGamesMap1:
                        totalGamesMap1[dirdir[i]] = 0
                    if dirdir[i] not in studentWinMap1:
                        studentWinMap1[dirdir[i]] = 0
                    if dirdir[i] not in trickerWinMap1:
                        trickerWinMap1[dirdir[i]] = 0
                    if dirdir[i] not in studentTotalGame1:
                        studentTotalGame1[dirdir[i]] = 0
                    if dirdir[i] not in trickerTotalGame1:
                        trickerTotalGame1[dirdir[i]] = 0

                totalGamesMap[dirdir[0]] += 1
                totalGamesMap[dirdir[1]] += 1
                studentTotalGame[dirdir[0]] += 1
                trickerTotalGame[dirdir[1]] += 1

                totalGameNum += 1

                if result["Student"] < result["Tricker"]:
                    winGamesMap[dirdir[1]] += 1
                    trickerWinMap[dirdir[1]] += 1
                    trickerWinNum += 1

                elif result["Student"] > result["Tricker"]:
                    winGamesMap[dirdir[0]] += 1
                    studentWinMap[dirdir[0]] += 1
                    studentWinNum += 1

                else:
                    winGamesMap[dirdir[0]] += 0.5
                    winGamesMap[dirdir[1]] += 0.5

                if result["Student"] != 0 and result["Tricker"] != 0:

                    totalGameNum1 += 1
                    totalGamesMap1[dirdir[0]] += 1
                    totalGamesMap1[dirdir[1]] += 1
                    studentTotalGame1[dirdir[0]] += 1
                    trickerTotalGame1[dirdir[1]] += 1

                    if result["Student"] < result["Tricker"]:
                        winGamesMap1[dirdir[1]] += 1
                        trickerWinMap1[dirdir[1]] += 1
                        trickerWinNum1 += 1

                    elif result["Student"] > result["Tricker"]:
                        winGamesMap1[dirdir[0]] += 1
                        studentWinMap1[dirdir[0]] += 1
                        studentWinNum1 += 1

                    else:
                        winGamesMap1[dirdir[0]] += 0.5
                        winGamesMap1[dirdir[1]] += 0.5

        except:
            pass

for i in totalGamesMap:
    winRateMap[i] = winGamesMap[i] / totalGamesMap[i]
    if studentTotalGame[i] == 0:
        studentWinRateMap[i] = 0
    else:
        studentWinRateMap[i] = studentWinMap[i] / studentTotalGame[i]
    if trickerTotalGame[i] == 0:
        trickerWinRateMap[i] = 0
    else:
        trickerWinRateMap[i] = trickerWinMap[i] / trickerTotalGame[i]


sortedMap = sorted(winRateMap.items(), key=lambda kv: (
    kv[1], kv[0]), reverse=True)

print("************************ALL GAMES(with 0 player)************************")

for i in sortedMap:
    width = 33 - len(re.findall('([\u4e00-\u9fa5])', teamIDtoName[i[0]]))
    print(
        f"Team {teamIDtoName[i[0]]:{width}}({i[0]}) wins {winGamesMap[i[0]]:4}/{totalGamesMap[i[0]]:<2} games({i[1]:.3f}), student wins {studentWinMap[i[0]]:2}/{studentTotalGame[i[0]]:<2}({studentWinRateMap[i[0]]:.3f}), tricker wins {trickerWinMap[i[0]]:2}/{trickerTotalGame[i[0]]:<2}({trickerWinRateMap[i[0]]:.3f})")

print(
    f"Total games: {totalGameNum}, student wins {studentWinNum}, tricker wins {trickerWinNum}")

for i in totalGamesMap1:
    if totalGamesMap1[i] == 0:
        winRateMap1[i] = 0
    else:
        winRateMap1[i] = winGamesMap1[i] / totalGamesMap1[i]
    if studentTotalGame1[i] == 0:
        studentWinRateMap1[i] = 0
    else:
        studentWinRateMap1[i] = studentWinMap1[i] / studentTotalGame1[i]
    if trickerTotalGame1[i] == 0:
        trickerWinRateMap1[i] = 0
    else:
        trickerWinRateMap1[i] = trickerWinMap1[i] / trickerTotalGame1[i]


sortedMap1 = sorted(winRateMap1.items(), key=lambda kv: (
    kv[1], kv[0]), reverse=True)

print("************************NON-0 GAMES(no 0 player)************************")

for i in sortedMap1:
    width = 33 - len(re.findall('([\u4e00-\u9fa5])', teamIDtoName[i[0]]))
    print(f"Team {teamIDtoName[i[0]]:{width}}({i[0]}) wins {winGamesMap1[i[0]]:4}/{totalGamesMap1[i[0]]:<2} games({i[1]:.3f}), student wins {studentWinMap1[i[0]]:2}/{studentTotalGame1[i[0]]:<2}({studentWinRateMap1[i[0]]:.3f}), tricker wins {trickerWinMap1[i[0]]:2}/{trickerTotalGame1[i[0]]:<2}({trickerWinRateMap1[i[0]]:.3f})")

print(
    f"Total games: {totalGameNum1}, student wins {studentWinNum1}, tricker wins {trickerWinNum1}")
