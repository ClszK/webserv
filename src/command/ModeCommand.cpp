#include "command/ModeCommand.hpp"

/**
 * - i : 초대 전용 채널 설정 / 제거
 * - t : 채널 운영자에 대한 TOPIC 명령 제한 설정 / 제거
 * - k : 채널 키(비밀번호) 설정 / 제거
 * - o : 채널 운영자 권한 부여 / 받기
 * - l : 채널의 유저 제한을 설정 / 제거
 *
 * MODE &oulu +b *!*@*.edu +e *!*@*.bu.edu
 * MODE #bu +be *!*@*.edu *!*@*.bu.edu
 *
 *
 * t2!root@192.168.65.1 MODE #1234 +k :1234
 */
std::string ModeCommand::execute(Client& client, Message& message) {
  if (message.params.size() < 1)
    return ReplyUtility::makeErrNeedMoreParamsReply(client, "MODE");

  std::string channelName = message.params[0];

  Channel* channel = Channel::findChannel(channelName);
  if (channel == NULL)
    return ReplyUtility::makeErrNoSuchChannelReply(client, channelName);
  if (message.params.size() == 1)
    return ReplyUtility::makeChannelModeIsReply(client, *channel);

  size_t paramIdx = 2, addMode = 1;
  std::string channelSetMode = message.params[1];
  std::string replyStr = "", nickName;
  Client* target = NULL;
  std::string plusMode = "";
  std::string removeMode = "";

  for (size_t i = 0; i < channelSetMode.size(); i++) {
    if (channelSetMode[i] == '+') {
      addMode = 1;
      continue;
    } else if (channelSetMode[i] == '-') {
      addMode = 0;
      continue;
    }
    char mode = channelSetMode[i];

    if (mode != 'o' && ((addMode && channel->getChannelMode().find(mode) !=
                                        std::string::npos) ||
                        (!addMode && channel->getChannelMode().find(mode) ==
                                         std::string::npos)))
      continue;
    switch (mode) {
      case 'i':
      case 't':
        if (addMode) {
          channel->setChannelModeAdd(mode);
          plusMode += mode;
        } else {
          channel->setChannelModeSub(mode);
          removeMode += mode;
        }
        break;
      case 'k':
        if (message.params.size() <= paramIdx) {
          replyStr +=
              ReplyUtility::makeErrNotExistReply(client, channelName, mode);
          break;
        }
        if (addMode) {
          channel->setChannelKey(message.params[paramIdx++]);
          channel->setChannelModeAdd(mode);
          plusMode += mode;
        } else {
          if (channel->getChannelKey() != message.params[paramIdx]) {
            replyStr +=
                ReplyUtility::makeErrKeyAlreadySetReply(client, channelName);
            break;
          }
          channel->setChannelKey("");
          channel->setChannelModeSub(mode);
          removeMode += mode;
        }
        break;
      case 'o':
        if (message.params.size() <= paramIdx) {
          replyStr +=
              ReplyUtility::makeErrNotExistReply(client, channelName, mode);
          break;
        }
        nickName = message.params[paramIdx++];
        target = Client::findClient(nickName);

        if (addMode) {
          if (target == NULL) {
            replyStr += ReplyUtility::makeErrNoSuchNickReply(client, nickName);
            break;
          }
          channel->setGMListAdd(*target);
          plusMode += mode;
        } else {
          channel->setGMListSub(*target);
          removeMode += mode;
        }
        break;
      case 'l':
        if (addMode) {
          if (message.params.size() <= paramIdx) {
            replyStr +=
                ReplyUtility::makeErrNotExistReply(client, channelName, mode);
            break;
          }
          channel->setChannelModeAdd(mode);
          channel->setMaxUser(
              std::strtol(message.params[paramIdx++].c_str(), NULL, 10));
          plusMode += mode;
        } else {
          channel->setChannelModeSub(mode);
          channel->setMaxUser(0);
          removeMode += mode;
        }
        break;
      default:
        replyStr += ReplyUtility::makeErrUnknownModeReply(client, mode);
    }
  }
  return replyStr;
}