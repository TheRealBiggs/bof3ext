// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace Silk.NET.Core.Loader;


public class DefaultPathResolver : PathResolver {
	public static readonly Func<string, IEnumerable<string>> PassthroughResolver = name
		=> Enumerable.Repeat(name, 1);

	public static readonly Func<string, IEnumerable<string>> BaseDirectoryResolver = name
		=> String.IsNullOrEmpty(AppContext.BaseDirectory) && String.IsNullOrWhiteSpace(Path.GetDirectoryName(name))
			? Enumerable.Empty<string>()
			: Enumerable.Repeat(Path.Combine(AppContext.BaseDirectory, name), 1);


	public List<Func<string, IEnumerable<string>>> Resolvers { get; set; } = [PassthroughResolver, BaseDirectoryResolver];


	public override IEnumerable<string> EnumeratePossibleLibraryLoadTargets(string name) {
		List<string> candidates = new();

		foreach (var resolver in Resolvers) {
			try {
				if (candidates.Count == 0 || resolver == PassthroughResolver) {
					candidates.AddRange(resolver.Invoke(name));
				} else {
					for (var i = 0; i < candidates.Count; i++) {
						var oldCount = candidates.Count;
						candidates.InsertRange(i + 1, resolver.Invoke(candidates[i]));
						i += candidates.Count - oldCount;
					}
				}
			} catch {
				// Skip failed resolvers, as in the upstream implementation.
			}
		}

		return candidates.Distinct();
	}
}
