from __future__ import (absolute_import, division, print_function)
from recon.helper import Helper
import numpy as np


def _apply_normalise(data, dark=None, norm_divide=None, clip_min=None, clip_max=None):
    return np.clip(np.true_divide(
        data - dark, norm_divide), clip_min, clip_max)


def execute(data, norm_flat_img=1, norm_dark_img=0, clip_min=0, clip_max=1.5, cores=1, chunksize=None, h=None):
    h = Helper.empty_init() if h is None else h
    h.check_data_stack(data)

    if norm_flat_img is not None and norm_dark_img is not None and isinstance(norm_flat_img, np.ndarray):
        if 2 != len(norm_flat_img.shape):
            raise ValueError(
                "Incorrect shape of the flat image ({0}) which should match the "
                "shape of the sample images ({1})".format(
                    norm_flat_img.shape, data[0].shape))

        if h.multiprocessing_available():
            execute_par(data, norm_flat_img, norm_dark_img,
                        clip_min, clip_max, cores, chunksize, h)
        else:
            execute_seq(data, norm_flat_img, norm_dark_img,
                        clip_min, clip_max, h)

    else:
        h.tomo_print_note(
            "Cannot apply normalization by flat/dark images because no valid flat image has been "
            "provided with -iflat/--input-path-flat")

    h.check_data_stack(data)
    return data


def execute_par(data, norm_flat_img=1, norm_dark_img=0, clip_min=0, clip_max=1.5, cores=1, chunksize=None, h=None):
    """
    Normalise by flat and dark images

    :param data :: image stack as a 3d numpy array
    :param norm_flat_img :: flat (open beam) image to use in normalization
    :param norm_dark_img :: dark image to use in normalization
    :param clip_min: Pixel values found below this value will be clipped to equal this value
    :param clip_max: Pixel values found above this value will be clipped to equal this value
    :param h: Helper class, if not provided will be initialised with empty constructor


    :returns :: filtered data (stack of images)
    """
    import numpy as np

    h.pstart(
        "Starting PARALLEL normalization by flat/dark images, pixel data type: {0}...".format(data.dtype))

    norm_divide = np.subtract(norm_flat_img, norm_dark_img)

    # prevent divide-by-zero issues
    norm_divide[norm_divide == 0] = 1e-6

    from functools import partial
    f = partial(_apply_normalise, dark=norm_dark_img,
                norm_divide=norm_divide, clip_min=clip_min, clip_max=clip_max)

    data = Helper.execute_async(
        data, f, cores, chunksize, "Norm by Flat/Dark", h)

    h.pstop(
        "Finished PARALLEL normalization by flat/dark images, pixel data type: {0}.".format(data.dtype))

    return data


def execute_not(data, norm_flat_img=1, norm_dark_img=0, clip_min=0, clip_max=1.5, h=None):
    """
    Normalise by flat and dark images

    :param data :: image stack as a 3d numpy array
    :param norm_flat_img :: flat (open beam) image to use in normalization
    :param norm_dark_img :: dark image to use in normalization
    :param clip_min: Pixel values found below this value will be clipped to equal this value
    :param clip_max: Pixel values found above this value will be clipped to equal this value
    :param h: Helper class, if not provided will be initialised with empty constructor


    :returns :: filtered data (stack of images)
    """
    import numpy as np

    h.pstart(
        "Starting normalization by flat/dark images, pixel data type: {0}...".format(data.dtype))

    norm_divide = np.subtract(norm_flat_img, norm_dark_img)

    # prevent divide-by-zero issues
    norm_divide[norm_divide == 0] = 1e-6

    # this divide gives bad results
    h.prog_init(data.shape[0], "Norm by Flat/Dark")
    for idx in range(0, data.shape[0]):
        data[idx, :, :] = np.clip(np.true_divide(
            data[idx, :, :] - norm_dark_img, norm_divide), clip_min, clip_max)
        h.prog_update(1)

    h.prog_close()
    h.pstop(
        "Finished normalization by flat/dark images, pixel data type: {0}.".format(data.dtype))

    return data