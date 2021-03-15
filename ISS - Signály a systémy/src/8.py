import numpy as np
from scipy.io import wavfile
from matplotlib import pyplot as plt
from scipy import signal
#   READS WAVS -> All FSs = 16000
fs_maskon_sentence, data_maskon_sentence = wavfile.read('../audio/maskon_sentence.wav')
fs_maskooff_sentence, data_maskoff_sentence = wavfile.read('../audio/maskoff_sentence.wav')
fs_maskon_tone, data_maskon_tone = wavfile.read('../audio/maskon_tone.wav')
fs_maskooff_tone, data_maskoff_tone = wavfile.read('../audio/maskoff_tone.wav')
#       LOAD DATA TO ARRAY
maskon_sentence_arr = np.array(data_maskon_sentence)
maskoff_sentence_arr = np.array(data_maskoff_sentence)
maskon_tone_arr = np.array(data_maskon_tone)
maskoff_tone_arr = np.array(data_maskoff_tone)
# GET 1.01 SECOND FROM EVERY WAV
on_sent = maskon_sentence_arr[3200:19360]
off_sent = maskoff_sentence_arr[3200:19360]
on_tone = maskon_tone_arr[25000:41160]
off_tone = maskoff_tone_arr[32432:48592]
# Array to store frames
on_sent_frames = []
off_sent_frames = []
on_tone_frames = []
off_tone_frames = []
#   CALCULATE MEAN OF EVERY SIGNAL
on_sent_mean = np.mean(on_sent)
off_sent_mean = np.mean(off_sent)
on_tone_mean = np.mean(on_tone)
off_tone_mean = np.mean(off_tone)

index = 0
for sample in on_sent:
    sample = sample - on_sent_mean
    on_sent[index] = sample
    index = index + 1
index = 0
for sample in off_sent:
    sample = sample - off_sent_mean
    off_sent[index] = sample
    index = index + 1
index = 0
for sample in on_tone:
    sample = sample - on_tone_mean
    on_tone[index] = sample
    index = index + 1
index = 0
for sample in off_tone:
    sample = sample - off_tone_mean
    off_tone[index] = sample
    index = index + 1

# COUNT MAX VALUES
on_sent_max = on_sent.max()
off_sent_max = off_sent.max()
on_tone_max = on_tone.max()
off_tone_max = off_tone.max()
# NORMALISATION
on_sent = on_sent / np.abs(on_sent).max()
off_sent = off_sent / np.abs(off_sent).max()
on_tone = on_tone / np.abs(on_tone).max()
off_tone = off_tone / np.abs(off_tone).max()
# Store signals into frames
for frame_counter in range(100):
    interval_start = 160 * frame_counter
    interval_end = 160 * frame_counter + 320
    on_sent_frames.append(on_sent[interval_start:interval_end])
    off_sent_frames.append(off_sent[interval_start:interval_end])
    on_tone_frames.append(on_tone[interval_start:interval_end])
    off_tone_frames.append(off_tone[interval_start:interval_end])

# START OF TASK 5
off_tone_k = np.zeros(32000).reshape((100, 320))
on_tone_k = np.zeros(32000).reshape((100, 320))
for frame in range(100):
    for k in range(320):
        off_tone_k[frame][k] = off_tone_frames[frame][k]
        on_tone_k[frame][k] = on_tone_frames[frame][k]

off_tone_fft = []
on_tone_fft = []
# calculate fft for every frame in maskon/maskoff tones
for frame in off_tone_frames:
    off_tone_sp = np.fft.fft(frame,1024)
    off_tone_fft.append(off_tone_sp)
for frame in on_tone_frames:
    on_tone_sp = np.fft.fft(frame,1024)
    on_tone_fft.append(on_tone_sp)

#freqency respond calculating - TASK 6
# div on_tone_fft / off_tone_fft
freq_res = np.divide(np.array(np.abs(on_tone_fft)),np.array(np.abs(off_tone_fft)))
# get the means samples, we use abs because complex
freq_average = np.abs(np.average(freq_res,axis=0))
freq_average[512:1024] = np.zeros(512)
#TASK 7
impulse_resp = np.fft.ifft(freq_average)
impulse_resp = np.real(impulse_resp[0:512])
#TASK 8
_,axs = plt.subplots(3,1)
axs[0].plot(data_maskoff_sentence)
axs[0].title.set_text("Veta bez rúška")
axs[1].plot(data_maskon_sentence)
axs[1].title.set_text("Veta s rúškom")
a0 = [1.0]
y_sim_tone = signal.lfilter(impulse_resp,a0,data_maskoff_tone)
y_sim_sentence = signal.lfilter(impulse_resp,a0,data_maskoff_sentence)
axs[2].plot(y_sim_sentence)
axs[2].title.set_text("Simulovaná veta")
plt.tight_layout()
plt.show()
# compare sim with without mask
plt.plot(data_maskoff_sentence, label="Veta bez rúška")
plt.plot(y_sim_sentence, label="Simulovaná veta")
plt.title("Porovnanie vety bez rúška so simulovanou vetou")
plt.xlabel("vzorky")
plt.legend(loc="upper right")
plt.show()
#compare sim with mask on
plt.plot(data_maskon_sentence, label="Veta s rúškom")
plt.plot(y_sim_sentence, label="Simulovaná veta")
plt.title("Porovnanie vety s rúškom / simulovaným rúškom")
plt.xlabel("vzorky")
plt.legend(loc="upper right")
plt.show()
#store to list and retype to int16
sample_out = y_sim_sentence.astype(np.int16)
tone_out = y_sim_tone.astype(np.int16)
wavfile.write("../audio/sim_maskon_sentence.wav",16000,sample_out)
wavfile.write("../audio/sim_maskon_tone.wav",16000,tone_out)
#https://stackoverflow.com/questions/8855574/convert-ndarray-from-float64-to-integer