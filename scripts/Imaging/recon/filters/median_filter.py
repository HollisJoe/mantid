from __future__ import (absolute_import, print_function, division)
from recon.helper import Helper


def import_scipy_ndimage():
    """
    Tries to import scipy so that the median filter can be applied
    :return:
    """

    try:
        import scipy.ndimage as scipy_ndimage
    except ImportError:
        raise ImportError(
            "Could not find the subpackage scipy.ndimage, required for image pre-/post-processing"
        )

    return scipy_ndimage


def execute(data, size, mode, cores=1, chunksize=None, h=None):
    h = Helper.empty_init() if h is None else h
    h.check_data_stack(data)

    if size and size > 1:
        if h.multiprocessing_available():
            data = execute_par(data, size, mode, cores, chunksize, h)
        else:
            data = execute_seq(data, size, mode, h)

    else:
        h.tomo_print_note("NOT applying noise filter / median.")

    h.check_data_stack(data)
    return data


def execute_seq(data, size, mode, h=None):
    """
    Sequential version of the Median Filter using scipy.ndimage

    :param data: The data that will be processed with this filter
    :param size: Size of the median filter kernel
    :param mode: Mode for the borders of the median filter. Options available in script -h command
    :param h: Helper class, if not provided will be initialised with empty constructor

    :return: Returns the processed data
    """
    h = Helper.empty_init() if h is None else h
    scipy_ndimage = import_scipy_ndimage()
    h.pstart(
        "Starting median filter, with pixel data type: {0}, filter size/width: {1}.".
        format(data.dtype, size))

    h.prog_init(data.shape[0], "Median filter")
    for idx in range(0, data.shape[0]):
        data[idx] = scipy_ndimage.median_filter(
            data[idx], size, mode=mode)
        h.prog_update()
    h.prog_close()

    h.pstop(
        "Finished median filter, with pixel data type: {0}, filter size/width: {1}.".
        format(data.dtype, size))


def execute_par(data, size, mode, cores=1, chunksize=None, h=None):
    """
    Parallel version of the Median Filter using scipy.ndimage

    :param data: The data that will be processed with this filter
    :param size: Size of the median filter kernel
    :param mode: Mode for the borders of the median filter. Options available in script -h command
    :param h: Helper class, if not provided will be initialised with empty constructor

    :return: Returns the processed data
    """

    h = Helper.empty_init() if h is None else h
    scipy_ndimage = import_scipy_ndimage()

    from functools import partial
    f = partial(scipy_ndimage.median_filter, size=size, mode=mode)

    h.pstart("Starting PARALLEL median filter")
    data = Helper.execute_async(data, f, cores, chunksize, "Median Filter", h)

    h.pstop("End of PARALLEL median filter")

    return data

# timeit.timeit(stmt='zoom(sample)', setup='from __main__ import sample, loader, zoom; import numpy as np; gc.enable()', number=100)

# timeit.timeit(stmt='execute_mp(data, size, mode)', setup='from __main__ import data, size, mode, h', number=10)
# timeit.timeit(stmt='execute_mp_prog(data, size, mode)', setup='from __main__ import data, size, mode, h', number=10)