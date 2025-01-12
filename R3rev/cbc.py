from pycbc import frame
paths = frame.frame_paths('H1:PEM-VAULT_MAG_1030X195Y_COIL_X_DQ', 1238166018, 1238170549+2048,server='nds.gwosc.org')
print(paths)