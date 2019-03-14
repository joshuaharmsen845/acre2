#include "SoundEngine.h"
#include "AcreSettings.h"
#include "Engine.h"

typedef std::numeric_limits<short int> LIMITER;

CSoundEngine::CSoundEngine( void ) {
    this->soundMixer = new CSoundMixer();
}

acre_result_t CSoundEngine::onEditPlaybackVoiceDataEvent(acre_id_t id, short* samples, int sampleCount, int channels) {
    if (CEngine::getInstance()->getSoundSystemOverride())
        return acre_result_ok;
    if (!CEngine::getInstance()->getGameServer())
        return acre_result_error;
    if (!CEngine::getInstance()->getGameServer()->getConnected())
        return acre_result_error;
    CPlayer *player;

    for (int x = 0; x < sampleCount * channels; x += channels) {
        for (int i = 0; i < channels; i++) {
            float result = static_cast<float>(samples[x + i]) * CAcreSettings::getInstance()->getPremixGlobalVolume();
            
            if (result > LIMITER::max()) result = LIMITER::max();
            else if (result < LIMITER::min()) result = LIMITER::min();
            
            samples[x + i] = static_cast<short>(result);
        }
    }

    auto it = CEngine::getInstance()->speakingList.find(id);
    CEngine::getInstance()->getSoundEngine()->getSoundMixer()->lock();
    if (it != CEngine::getInstance()->speakingList.end()) {
        player = (CPlayer *)it->second;
        LOCK(player);
        if (player->getSpeakingType() != acre_speaking_unknown) {
            for (size_t i = 0; i < player->channels.size(); ++i) {
                if (player->channels[i]) {
                    player->channels[i]->lock();
                    player->channels[i]->In(samples, sampleCount);
                    player->channels[i]->unlock();
                }
            }
        } else {
            memset(samples, 0x00, (sampleCount*channels)*sizeof(short) );
        }


        UNLOCK(player);
    } else {
        memset(samples, 0x00, (sampleCount*channels)*sizeof(short) );
    }
    CEngine::getInstance()->getSoundEngine()->getSoundMixer()->unlock();
    return acre_result_ok;
}

acre_result_t CSoundEngine::onEditPostProcessVoiceDataEvent(acre_id_t id, short* samples, int sampleCount, int channels, const unsigned int* channelSpeakerArray, unsigned int* channelFillMask) {
    
    if (CEngine::getInstance()->getSoundSystemOverride())
        return acre_result_ok;
    if (!CEngine::getInstance()->getGameServer())
        return acre_result_error;
    if (!CEngine::getInstance()->getGameServer()->getConnected())
        return acre_result_error;
    memset(samples, 0x00, (sampleCount*channels)*sizeof(short) );
    *channelFillMask = (1<<channels)-1;
    return acre_result_ok;
}

acre_result_t CSoundEngine::onEditMixedPlaybackVoiceDataEvent(short* samples, int sampleCount, int channels, const unsigned int speakerMask) {
    if (CEngine::getInstance()->getSoundSystemOverride())
        return acre_result_ok;
    if (!CEngine::getInstance()->getGameServer())
        return acre_result_error;
    if (!CEngine::getInstance()->getGameServer()->getConnected())
        return acre_result_error;
    memset(samples, 0x00, (sampleCount*channels)*sizeof(short) );
    this->getSoundMixer()->mixDown(samples, sampleCount, channels, speakerMask);

    for (int x = 0; x < sampleCount * channels; x += channels) {
        for (int i = 0; i < channels; i++) {
            float result = static_cast<float>(samples[x + i]) * CAcreSettings::getInstance()->getGlobalVolume();

            if (result > LIMITER::max()) result = LIMITER::max();
            else if (result < LIMITER::min()) result = LIMITER::min();

            samples[x + i] = static_cast<short>(result);
        }
    }

    return acre_result_ok;
}

acre_result_t CSoundEngine::onEditCapturedVoiceDataEvent(short* samples, int sampleCount, int channels) {
    if (CEngine::getInstance()->getSoundSystemOverride())
        return acre_result_ok;
    if (!CEngine::getInstance()->getGameServer())
        return acre_result_error;
    if (!CEngine::getInstance()->getGameServer()->getConnected())
        return acre_result_error;
    /*
    if (CEngine::getInstance()->getSelf()) {
        if (CEngine::getInstance()->getSelf()->getSpeaking()) {
            CEngine::getInstance()->getSoundEngine()->getSoundMixer()->lock();
            CSelf *self = CEngine::getInstance()->getSelf();
            self->lock();
            for (int i = 0; i < self->channels.size(); ++i) {
                if (self->channels[i]) {
                    self->channels[i]->lock();
                    self->channels[i]->In(samples, sampleCount);
                    self->channels[i]->unlock();
                }
            }
            self->unlock();
            CEngine::getInstance()->getSoundEngine()->getSoundMixer()->unlock();
        }
    }
    */
    return acre_result_ok;
}