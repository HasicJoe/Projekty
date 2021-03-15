import numpy as np
from scipy.io import wavfile
from matplotlib import pyplot as plt

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
frame_with_mask = on_tone_frames[9]
frame_without_mask = off_tone_frames[9]

# FIRST GRAPH FRAME WITH MASKON
plt.title("Rámec")
plt.plot(frame_without_mask)
plt.xlabel("vzorky")
plt.ylabel("y")
plt.show()
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

# CHOSE ONE FRAME AFTER CENTER CLIPPING, I CHOSE 53th
frame_without_mask = off_tone_frames[9]
frame_with_mask = on_tone_frames[9]

plt.title('Clipping - Rámec')
plt.xlabel("vzorky")
plt.ylabel("y")
plt.plot(frame_without_mask)
plt.show()

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

# FRAME WITH LAG
prah = 32
fr_len = 320
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

# DISPLAY CORRELATED FRAME WITH LAG
plt.plot(result_off[9])
plt.title("Autokorelácia")
lag_x = lag_x_value_off[9]
lag_y = lag_y_value_off[9]
plt.stem([lag_x], [lag_y], 'r', label="LAG")
plt.axvline(x=prah, color="black", ymin=0.0, ymax=result_off[9].max(), label="Prah")
plt.xlabel("vzorky")
plt.ylabel("y")
plt.legend(loc="upper right")
plt.show()

frame_len = 320
frame_time = 0.02
f0_off = np.zeros(100)
f0_on = np.zeros(100)
for i in range(100):
    tmp_off = (lag_x_value_off[i]/frame_len)*frame_time
    f0_off[i] = 1/tmp_off
    tmp_on = (lag_x_value_on[i] / frame_len) * frame_time
    f0_on[i] = 1 / tmp_on

disp_off_tone = np.std(f0_off)
disp_off_tone = np.square(disp_off_tone)
disp_on_tone = np.std(f0_on)
disp_on_tone = np.square(disp_on_tone)
mean_off_tone = np.median(f0_off)
mean_on_tone = np.median(f0_on)
print("Rozptyl bez rúška:", disp_off_tone, "\nRozptyl s rúškom:",disp_on_tone)
print("\nStredná hodnota bez rúška:",mean_off_tone,"\nStredná hodnota s rúškom:",mean_on_tone)
plt.title("Základná frekvencia rámcov")
plt.plot(f0_on, label="signál s rúškom")
plt.plot(f0_off, label="signál bez rúška")
plt.legend(loc="upper right")
plt.xlabel("rámce")
plt.ylabel("f0")
plt.show()