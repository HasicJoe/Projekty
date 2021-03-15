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
def my_dft(array, leng):
    arr_len = len(array)
    Xk = np.zeros(leng, dtype=complex)
    for k in range(leng):
        for n in range(arr_len):
            Xk[k] += array[n]*np.exp(-2j*np.pi*k*n/leng)
    return Xk

#Testing
numpy_dft = np.fft.fft(np.exp(2j*np.pi*np.arange(8)/8),1024)
my_dft = my_dft(np.exp(2j*np.pi*np.arange(8)/8),1024)
print(np.allclose(numpy_dft,my_dft))

off_tone_fft = []
on_tone_fft = []

# calculate fft for every frame in maskon/maskoff tones
for frame in off_tone_frames:
    off_tone_sp = np.fft.fft(frame,1024)
    off_tone_sp = off_tone_sp[0:512]
    off_tone_fft.append(off_tone_sp)
for frame in on_tone_frames:
    on_tone_sp = np.fft.fft(frame,1024)
    on_tone_sp = on_tone_sp[0:512]
    on_tone_fft.append(on_tone_sp)

# create array to store Power
off_tone_P = np.zeros(51200).reshape((100, 512))
on_tone_P = np.zeros(51200).reshape((100, 512))
#tmp arrays
aux_off = []
aux_on = []

for frame in range(100):
    for k in range(512):
        aux_off.append(10*np.log10(np.abs(off_tone_fft[frame][k]**2)))
        aux_on.append(10*np.log10(np.abs((on_tone_fft[frame][k]**2))))
    off_tone_P[frame] = aux_off
    on_tone_P[frame] = aux_on
    aux_off = []
    aux_on = []

# transpose arrays
off_tone_P = off_tone_P.T
on_tone_P = on_tone_P.T
# plot spectograms
plt.imshow(on_tone_P, aspect="auto", origin="lower", extent=[0.0, 1.0, 0, 8000]) # origin = lower set index of the array in the upper left
plt.title("Spektogram s rúškou")
plt.xlabel("Čas")
plt.ylabel("Frekvencia")
cbar = plt.colorbar()
plt.tight_layout()
plt.show()
plt.imshow(off_tone_P, aspect="auto", origin="lower", extent=[0.0, 1.0, 0, 8000]) # origin = lower set index of the array in the upper left
cbar = plt.colorbar()
plt.title("Spektogram bez rúšky")
plt.xlabel("Čas")
plt.ylabel("Frekvencia")
plt.tight_layout()
plt.show()