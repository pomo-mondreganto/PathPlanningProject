from matplotlib.animation import FFMpegWriter


class FasterFFMpegWriter(FFMpegWriter):

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.frame_format = 'argb'

    def grab_frame(self, **savefig_kwargs):
        try:
            self.fig.set_size_inches(self._w, self._h)
            self.fig.set_dpi(self.dpi)
            self.fig.canvas.draw()
            self._frame_sink().write(self.fig.canvas.tostring_argb())
        except (RuntimeError, IOError) as e:
            out, err = self._proc.communicate()
            raise IOError('Error saving animation to file (cause: {0}) '
                          'Stdout: {1} StdError: {2}. It may help to re-run '
                          'with --verbose-debug.'.format(e, out, err))
