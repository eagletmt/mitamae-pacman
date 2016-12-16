MRuby::Gem::Specification.new('mitamae-pacman') do |spec|
  spec.license = 'MIT'
  spec.author = ['Kohei Suzuki']
  spec.summary = 'Provide MItamae::InlineBackends::Pacman for faster package management'
  spec.version = '0.2.0'

  spec.linker.libraries << 'alpm'
end
