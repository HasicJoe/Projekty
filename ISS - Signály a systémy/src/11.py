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

# TASK 11 - BLACKMAN WINDOW IN TIME
plt.plot(np.blackman(320))
plt.title("Blackmanova okienková funkcia v čase")
plt.xlabel("vzorky")
plt.ylabel("amplitúda")
plt.show()
# TASK 11 - BLACKMAN WINDOW IN THE SPECTRE
blackman_freq = np.fft.fft(np.blackman(320),1024)  # for this case i use only 32 because of better looking graph
freq = np.abs(np.fft.fftshift(blackman_freq))
plt.title("Blackmanova okienkova funkcia v spektre")
plt.ylabel("Magnitúda")
plt.plot(10*np.log10(np.abs((freq**2))))
plt.show()

off_tone_blackman_k = np.zeros(32000).reshape((100, 320))
on_tone_blackman_k = np.zeros(32000).reshape((100, 320))
for frame in range(100):
    off_tone_blackman_k[frame] = off_tone_k[frame] * np.blackman(320)
    on_tone_blackman_k[frame] = on_tone_k[frame] * np.blackman(320)

plt.plot(off_tone_blackman_k[10])
plt.plot(off_tone_frames[10])
plt.show()

off_tone_fft = []
on_tone_fft = []
off_tone_fft_blackman = []
on_tone_fft_blackman = []
# calculate fft for every frame in maskon/maskoff tones
for frame in off_tone_k:
    off_tone_sp = np.fft.fft(frame,1024)
    off_tone_fft.append(off_tone_sp)
for frame in on_tone_k:
    on_tone_sp = np.fft.fft(frame,1024)
    on_tone_fft.append(on_tone_sp)
for frame in on_tone_blackman_k:
    on_bltone_sp = np.fft.fft(frame,1024)
    on_tone_fft_blackman.append(on_bltone_sp)
for frame in off_tone_blackman_k:
    off_bltone_sp = np.fft.fft(frame,1024)
    off_tone_fft_blackman.append(off_bltone_sp)

# frame spectrum for and without blackman window function
off_without_bl = 10*np.log10(np.abs(off_tone_fft[22][0:512]**2))
plt.title("Porovnanie spektra rámca bez rúška")
plt.plot(off_without_bl, label="bez okienkovej funkcie")
off_bl = 10*np.log10(np.abs(off_tone_fft_blackman[22][0:512]**2))
plt.plot(off_bl, label="s Blackmanovou okienkovou funkciou")
plt.legend(loc="upper right")
plt.show()

on_without_bl = 10*np.log10(np.abs(on_tone_fft[23][0:512]**2))
plt.title("Porovnanie spektra rámca s rúškom")
plt.plot(on_without_bl, label="bez okienkovej funkcie")
on_bl = 10*np.log10(np.abs(on_tone_fft_blackman[23][0:512]**2))
plt.plot(on_bl, label="s Blackmanovou okienkovou funkciou")
plt.legend(loc="upper right")
plt.show()
#freqency respond calculating - TASK 6
# div on_tone_fft / off_tone_fft
freq_res = np.divide(np.array(np.abs(on_tone_fft_blackman)),np.array(np.abs(off_tone_fft_blackman)))
# get the means of 0 - N/2 samples, we use abs because complex
freq_average = np.abs(np.average(freq_res,axis=0))
freq_average[512:1024] = np.zeros(512)
#TASK 7
impulse_resp = np.fft.ifft(freq_average)
impulse_resp = np.real(impulse_resp[0:512])
#TASK 8
a0 = [1.0]
y_sim_tone = signal.lfilter(impulse_resp,a0,data_maskoff_tone)
y_sim_sentence = signal.lfilter(impulse_resp,a0,data_maskoff_sentence)
sample_out = y_sim_sentence.astype(np.int16)
tone_out = y_sim_tone.astype(np.int16)
wavfile.write("../audio/sim_maskon_sentence_window.wav",16000,sample_out)
wavfile.write("../audio/sim_maskon_tone_window.wav",16000,tone_out)