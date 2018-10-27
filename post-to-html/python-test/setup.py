from distutils.core import setup, Extension
setup(name = 'postToHtml', version = '1.0',  \
   ext_modules = [Extension('postToHtml', ['post_to_html.c'])])
