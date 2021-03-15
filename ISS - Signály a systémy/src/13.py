import numpy as np
from scipy.io import wavfile
from matplotlib import pyplot as plt
from scipy import signal
import copy

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

# Array to store frames
on_sent_frames = []
off_sent_frames = []
on_tone_frames = []
off_tone_frames = []

# Store signals into frames
for frame_counter in range(100):
    interval_start = 160 * frame_counter
    interval_end = 160 * frame_counter + 320
    on_sent_frames.append(on_sent[interval_start:interval_end])
    off_sent_frames.append(off_sent[interval_start:interval_end])
    on_tone_frames.append(on_tone[interval_start:interval_end])
    off_tone_frames.append(off_tone[interval_start:interval_end])

# START OF TASK 4 HERE
off_no_cor = copy.deepcopy(off_tone_frames) # save before autocorellation and clipping
on_no_cor = copy.deepcopy(on_tone_frames)

# CENTER CLIPPING
for frame in range(100):
    off_max = np.abs(off_tone_frames).max()
    on_max = np.abs(on_tone_frames).max()
    for sample in range(160):       # do it for first half of frame
        # do it for tone with mask
        if on_tone_frames[frame][sample] > 0.7 * on_max:
            on_tone_frames[frame][sample] = 1
        elif on_tone_frames[frame][sample] < -0.7 * on_max:
            on_tone_frames[frame][sample] = -1
        else:
            on_tone_frames[frame][sample] = 0
        # do it for tone without mask
        if off_tone_frames[frame][sample] > (0.7 * off_max):
            off_tone_frames[frame][sample] = 1
        elif off_tone_frames[frame][sample] < (-0.7 * off_max):
            off_tone_frames[frame][sample] = 1
        else:
            off_tone_frames[frame][sample] = 0

# LETS DO AUTOCORRELATION
result_off = np.zeros(32000).reshape((100, 320))
result_on = np.zeros(32000).reshape((100, 320))

# R[k] = sum(x[n]*x[n+k])
for frame in range(100):
    for k in range(320):
        sample_off = 0
        sample_on = 0
        for n in range(320):
            if n+k <= 319:
                sample_off += off_tone_frames[frame][n] * off_tone_frames[frame][n+k]
                sample_on += on_tone_frames[frame][n] * on_tone_frames[frame][n+k]
            else:
                n = 320
        result_off[frame][k] = sample_off
        result_on[frame][k] = sample_on

# START OF TASK 5
off_tone_k = np.zeros(32000).reshape((100, 320))
on_tone_k = np.zeros(32000).reshape((100, 320))
for frame in range(100):
    for k in range(320):
        off_tone_k[frame][k] = off_no_cor[frame][k]
        on_tone_k[frame][k] = on_no_cor[frame][k]

off_tone_fft = []
on_tone_fft = []
# calculate fft for every frame in maskon/maskoff tones
for frame in off_tone_k:
    off_tone_sp = np.fft.fft(frame,1024)
    off_tone_fft.append(off_tone_sp)

for frame in on_tone_k:
    on_tone_sp = np.fft.fft(frame,1024)
    on_tone_fft.append(on_tone_sp)

#freqency respond calculating - TASK 6
# div on_tone_fft / off_tone_fft
freq_res = np.divide(np.array(np.abs(on_tone_fft)),np.array(np.abs(off_tone_fft)))
# calculate freqency for 0-N/2 samples
freq = np.zeros(512)
for i in range(512):
    freq[i] = (i/512) * (fs_maskooff_tone/2)
# get the means of 0 - N/2 samples
freq_average = np.average(freq_res, axis=0)

for second_half in range(512):
    freq_average[int(512+second_half)]=np.zeros(1)
#plotting
plt.plot(freq,10*np.log10(np.abs(freq_average[0:512]**2)),label="klasická")
plt.title("Frekvenčná charakteristika rúška")
plt.xlabel("Frekvencia")
plt.ylabel("Spektrálna hustota výkonu")

# FRAME WITH LAG
fr_len = 320
prah = int(fr_len/10)
lag_x_value_off = np.zeros(100)
lag_x_value_on = np.zeros(100)
lag_y_value_off = np.zeros(100)
lag_y_value_on = np.zeros(100)

for frame in range(100):
    lag_x_off = (np.array(result_off[frame][prah:fr_len-prah]).argmax()) + prah
    lag_y_off = result_off[frame][lag_x_off]
    lag_x_value_off[frame] = lag_x_off
    lag_y_value_off[frame] = lag_y_off

    lag_x_on = np.array(result_on[frame][prah:fr_len - prah]).argmax() + prah
    lag_y_on = result_on[frame][lag_x_on]
    lag_x_value_on[frame] = lag_x_on
    lag_y_value_on[frame] = lag_y_on

frame_len = 320
frame_time = 0.02
f0_off = np.zeros(100)
f0_on = np.zeros(100)
for i in range(100):
    tmp_off = (lag_x_value_off[i]/frame_len)*frame_time
    f0_off[i] = 1/tmp_off
    tmp_on = (lag_x_value_on[i] / frame_len) * frame_time
    f0_on[i] = 1 / tmp_on

# OPRAVA VIACNASOBNEHO LAGU TASK 12 - NARAZILI SME NA CHYBU PRI TONE BEZ MASKY FRAMU 46
lag_x_median = np.median(lag_x_value_on)
for frame in range(100):
    if lag_x_value_on[frame] > (lag_x_median + prah):
        lag_x_value_on[frame] = lag_x_median
        lag_y_on = result_on[frame][int(lag_x_median)]
        lag_y_value_on[frame] = lag_y_on

    elif lag_x_value_on[frame] < (lag_x_median - prah):
        lag_x_value_on[frame] = lag_x_median
        lag_y_on = result_on[frame][int(lag_x_median)]
        lag_y_value_on[frame] = lag_y_on

# START OF TASK 13
samef0_off_tone = []
samef0_on_tone = []
for frame in range(100):
    if lag_x_value_off[frame] == lag_x_value_on[frame]:
        samef0_off_tone.append(off_no_cor[frame])
        samef0_on_tone.append(on_no_cor[frame])
# now we stored frames with same f0 next step is fft

off_tone_fft_13 = []
on_tone_fft_13 = []
# calculate fft for every frame in maskon/maskoff tones
for frame in samef0_off_tone:
    off_tone_sp = np.fft.fft(frame,1024)
    off_tone_fft_13.append(off_tone_sp)
for frame in samef0_on_tone:
    on_tone_sp = np.fft.fft(frame,1024)
    on_tone_fft_13.append(on_tone_sp)

#freqency respond calculating - TASK 6
# div on_tone_fft / off_tone_fft
freq_res_13 = np.divide(np.array(np.abs(on_tone_fft_13)),np.array(np.abs(off_tone_fft_13)))
# get the means of 0 - N/2 samples
freq_average_13 = np.average(freq_res_13,axis=0)

#plotting
plt.plot(freq,10*np.log10(np.abs(freq_average_13[0:512]**2)),label="s rámcami s rovnakým f0")
plt.legend(loc="upper right")
plt.show()
impulse_resp = np.fft.ifft(freq_average)
impulse_resp = np.real(impulse_resp[0:512])

#TASK 8
a0 = [1.0]
y_sim_tone = signal.lfilter(impulse_resp,a0,data_maskoff_tone)
y_sim_sentence = signal.lfilter(impulse_resp,a0,data_maskoff_sentence)
sample_out = y_sim_sentence.astype(np.int16)
tone_out = y_sim_tone.astype(np.int16)
wavfile.write("../audio/sim_maskon_sentence_only_match.wav",16000,sample_out)
wavfile.write("../audio/sim_maskon_tone_only_match.wav",16000,tone_out)