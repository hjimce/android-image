#coding=utf-8
MODEL_FILE = 'model/para.caffemodel'
# .prototxt file path:
MODEL_NET = 'model/net.prototxt'
MEAN_FILE='model/mean.binaryproto'
# Saving path:
SAVE_TO = 'model/paras/'

# Set True if you want to get parameters:
GET_PARAMS = True
# Set True if you want to get blobs:
GET_BLOBS = False

import sys
import os
import numpy as np
import msgpack
import cv2

# Make sure that Caffe is on the python path:
# Caffe installation path:
caffe_root = '/home/hjimce/tools/caffe/'
sys.path.insert(0, caffe_root + 'python')
sys.path.append(os.path.abspath('.'))
import caffe


def get_blobs(net):
    net_blobs = {'info': 'net.blobs data'}
    for key in net.blobs.iterkeys():
        print('Getting blob: ' + key)
        net_blobs[key] = net.blobs[key].data

    return net_blobs

def get_params(net):
    net_params = {'info': 'net.params data'}
    for key in net.params.iterkeys():
        print('Getting parameters: ' + key)
        if type(net.params[key]) is not caffe._caffe.BlobVec:
            net_params[key] = net.params[key].data
        else:
            net_params[key] = [net.params[key][0].data, net.params[key][1].data]

    return net_params

image=cv2.imread("5.jpg")
cv2.imwrite("5.jpg",cv2.resize(image,(39,39)))
image=cv2.imread("5.jpg")
image=cv2.cvtColor(image,cv2.COLOR_BGR2RGB)
image=np.asarray([np.transpose(image,(2,0,1))])

# 模型加载
caffe.set_mode_cpu()
net = caffe.Net(MODEL_NET, MODEL_FILE, caffe.TEST)
mean_blob = caffe.proto.caffe_pb2.BlobProto()
mean_blob.ParseFromString(open(MEAN_FILE, 'rb').read())
# 将均值blob转为numpy.array
mean_npy = caffe.io.blobproto_to_array(mean_blob)[:,:,:39,:39]
buf = msgpack.packb(mean_npy.tolist(), use_single_float = True)
with open(SAVE_TO + 'model_mean' + '.msg', 'wb') as f:
    f.write(buf)





net.blobs['data'].data[...] =image-mean_npy
out = net.forward()
for k,v in net.blobs.items():
	if len(v.data.shape)==4:
		print k,v.data.shape,v.data[0,0,0,0]
	else:
		print k,v.data.shape,v.data[0,0]
print out
# Extract the model:
if GET_BLOBS:
    blobs = get_blobs(net)
if GET_PARAMS:
    params = get_params(net)

save_list = []

# Write blobs:
if GET_BLOBS:
    save_list.append('***BLOBS***')
    for b in blobs.iterkeys():
        print('Saving blob: ' + b)
        if type(blobs[b]) is np.ndarray:
            save_list.append(b)
            buf = msgpack.packb(blobs[b].tolist(), use_single_float = True)
            with open(SAVE_TO + 'model_blob_'+ b + '.msg', 'wb') as f:
                f.write(buf)
        else:
            print('Blob ' + b + ' not saved.')

# Write parameters:
if GET_PARAMS:
    save_list.append('***PARAMS***')
    for b in params.iterkeys():
        print('Saving parameters: ' + b)
        if type(params[b]) is np.ndarray:
            save_list.append(b)
            buf = msgpack.packb(params[b].tolist(), use_single_float = True)
            with open(SAVE_TO + 'model_param_'+ b + '.msg', 'wb') as f:
                f.write(buf)
        elif type(params[b]) is list:
            save_list.append(b)
            if len(params[b][0].shape) == 4:			# for the convolution layers
               buf1 = msgpack.packb(params[b][0].tolist(), use_single_float = True)
            elif len(params[b][0].shape) == 2:			# for the fully-connected layers
               buf1 = msgpack.packb(params[b][0].ravel().tolist(), use_single_float = True)
            buf2 = msgpack.packb(params[b][1].tolist(), use_single_float = True)
            with open(SAVE_TO + 'model_param_'+ b + '.msg', 'wb') as f:
                f.write(buf1)
                f.write(buf2)
        else:
            print('Parameters ' + b + ' not saved.')

print('Saved data:')
print(save_list)
